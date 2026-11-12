#include "mee/components/core/transform.hpp"
#include "mee/components/physics/aabb-collider.hpp"
#include "mee/components/physics/rigidbody.hpp"
#include "mee/components/render/sprite.hpp"
#include "mee/core/signal.hpp"
#include "mee/ecs/ecs.hpp"
#include "mee/core/engine.hpp"
#include "mee/render/texture-manager.hpp"
#include "mee/systems/render/render-sprite.hpp"
#include "mee/utils/color.hpp"
#include "mee/utils/vector.hpp"
#include "mee/engine-resources/engine-resources.hpp"
#include "mee/systems/render/camera-target.hpp"
#include "mee/systems/physics/aabb-collision.hpp"
#include "mee/systems/physics/move.hpp"
#include "mee/components/tags/camera-target.hpp"
#include <cstdio>
#include <raylib.h>
#include <string>

struct PlayerTagComponent{

};

struct HealthComponent{
    int health;
    Signal<int> damage;
};

int main(){
    Engine w;
    w.config.debug.enabled = false;
    w.config.debug.showColliders = true;
    w.init();

    Entity player = w.ecs.createEntity(
        TransformComponent{
            .scale = Vector2f(15.0f, 15.0f)
        },
        SpriteComponent{
            .texture = w.res.textureManager.loadTexture("examples/basic/sprite/assets/dirt.png"),
            .tileSize = Vector2f(1.0f, 1.0f)
        },
        PlayerTagComponent{},
        RigidbodyComponent{
            .drag = 5.0
        },
        CameraTargetTag{},
        AABBColliderComponent{
            .halfSize = Vector2f(8.0f, 8.0f)
        },
        HealthComponent{
            .health = 100,
        },
        CameraTargetTag{}
    );
    Entity wall = w.ecs.createEntity(
        TransformComponent{},
        SpriteComponent{
            .texture = w.res.textureManager.loadTexture("examples/basic/sprite/assets/texture.png")
        },
        RigidbodyComponent{},
        AABBColliderComponent{
            .halfSize = Vector2f(135.0f, 157.0f)
        }
    );

    w.pipeline.onStart(
        [](ECS& ecs, EngineResources& res){
            ecs.view<HealthComponent>(
                [&](Entity e, HealthComponent& hl){
                    HealthComponent* hlp = &hl;
                    hl.damage.connect(
                        [hlp, &ecs, e](int damge){
                            hlp->health -= damge;
                            if(hlp->health <= 0){
                                ecs.removeEntity(e);
                            }
                        }
                    );
                }
            );
        }
    );
    w.pipeline.onDraw(renderSpriteSystem);
    w.pipeline.onDraw(
        [](ECS& ecs, EngineResources& res){
            ecs.view<HealthComponent, TransformComponent, SpriteComponent>(
                [&](Entity e, HealthComponent& hl, TransformComponent& tr, SpriteComponent& sp){
                    TextureMetadata textureMetadata = res.textureManager.getTextureMetadata(sp.texture);
                    res.renderer.renderText(
                        {
                            std::to_string(hl.health),
                            tr.pos + Vector2f(0, -textureMetadata.size.y * tr.scale.y / 2.0f),
                            20,
                            Color4f::Red()
                        },
                        20
                    );
                }
            );
        }
    );
    w.res.collisionSignal.connect(
        [&](Entity a, Entity b){
            if(w.ecs.hasComponent<HealthComponent>(a) && w.ecs.hasComponent<TransformComponent>(a)){
                auto& hl = w.ecs.getComponent<HealthComponent>(a);
                hl.damage.emit(10);
            }
            if(w.ecs.hasComponent<HealthComponent>(b) && w.ecs.hasComponent<TransformComponent>(b)){
                auto& hl = w.ecs.getComponent<HealthComponent>(b);
                hl.damage.emit(10);
            }
        }
    );
    w.pipeline.onUpdate(
        [&](ECS& ecs, EngineResources &res, float dt){
            ecs.view<RigidbodyComponent, PlayerTagComponent>(
                [&](Entity e, RigidbodyComponent& r, auto& tag){
                    float walkSpeed = 1000.0f;
                    if(res.input.isKeyDown(KEY_A)){
                        r.velocity.x -= walkSpeed * dt;
                    }
                    if(res.input.isKeyDown(KEY_D)){
                        r.velocity.x += walkSpeed * dt;
                    }
                    if(res.input.isKeyDown(KEY_W)){
                        r.velocity.y -= walkSpeed * dt;
                    }
                    if(res.input.isKeyDown(KEY_S)){
                        r.velocity.y += walkSpeed * dt;
                    }
                    if(res.input.isKeyDown(KEY_Q)){
                        res.camera.zoom /= 1.05f;
                    }
                    if(res.input.isKeyDown(KEY_E)){
                        res.camera.zoom *= 1.05f;
                    }
                    if(res.input.isKeyDown(KEY_R)){
                        SpriteComponent& s = ecs.getComponent<SpriteComponent>(player);
                        s.tileSize.x /= 1.01f;
                        s.tileSize.y /= 1.01f;
                    }
                }
            );
        }
    );
    w.pipeline.onUpdate(moveSystem);
    w.pipeline.onUpdate(aabbCollideSystem);
    w.pipeline.onUpdate(cameraTargetSystem);

    w.run();
}
