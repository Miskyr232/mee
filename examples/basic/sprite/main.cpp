#include "mee/components/core/transform.hpp"
#include "mee/components/physics/aabb-collider.hpp"
#include "mee/components/physics/rigidbody.hpp"
#include "mee/components/render/sprite.hpp"
#include "mee/ecs/ecs.hpp"
#include "mee/core/world.hpp"
#include "mee/systems/render/render-sprite.hpp"
#include "mee/utils/vector.hpp"
#include "mee/engine-resources/engine-resources.hpp"
#include "mee/systems/render/camera-target.hpp"
#include "mee/systems/physics/aabb-collision.hpp"
#include "mee/systems/physics/move.hpp"
#include "mee/components/tags/camera-target.hpp"
#include <raylib.h>

struct PlayerTagComponent{

};

int main(){
    Engine w;
    w.config.debug.enabled = true;
    w.config.debug.showColliders = true;
    w.init();

    Entity player = w.ecs.createEntity(
        TransformComponent{
            .scale = Vector2f(2.0f, 1.0f)
        },
        SpriteComponent{
            .texture = w.res.textureManager.loadTexture("examples/basic/sprite/assets/texture.png"),
            .tileSize = Vector2f(1.0f, 1.0f)
        },
        PlayerTagComponent{},
        RigidbodyComponent{
            .drag = 5.0
        },
        CameraTargetTag{},
        AABBColliderComponent{
            .halfSize = Vector2f(135.0f, 157.0f)
        }
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

    w.pipeline.onDraw(renderSpriteSystem);
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
                    if(res.input.isKeyDown(KEY_E)){
                        res.camera.zoom *= 1.05f;
                    }
                    if(res.input.isKeyDown(KEY_Q)){
                        res.camera.zoom /= 1.05f;
                    }
                    if(res.input.isKeyPressed(KEY_R)){
                        SpriteComponent& s = ecs.getComponent<SpriteComponent>(player);
                        s.tileSize /= 2.0f;
                    }
                }
            );
        }
    );
    w.pipeline.onUpdate(moveSystem);
    w.pipeline.onUpdate(aabbCollideSystem);

    w.run();
}
