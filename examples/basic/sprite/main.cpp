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
#include <cstdio>
#include <filesystem>
#include <raylib.h>

struct PlayerTagComponent{

};

int main(){
    Engine w;
    w.config.debug.enabled = true;
    w.config.debug.showColliders = true;
    w.init();

    Entity player = w.ecs.createEntity(
        TransformComponent{},
        SpriteComponent{
            .texture = w.res.textureManager.loadTexture("examples/basic/sprite/assets/texture.png")
        },
        PlayerTagComponent{},
        RigidbodyComponent{},
        CameraTargetTag{},
        AABBColliderComponent{
            .halfSize = Vector2d(64.0, 64.0)
        }
    );
    Entity wall = w.ecs.createEntity(
        TransformComponent{},
        SpriteComponent{
            .texture = w.res.textureManager.loadTexture("examples/basic/sprite/assets/texture.png")
        },
        RigidbodyComponent{},
        AABBColliderComponent{
            .halfSize = Vector2d(64.0, 64.0)
        }
    );

    w.pipeline.onDraw(renderSpriteSystem);
    w.pipeline.onUpdate(
        [](ECS& ecs, EngineResources &res, float dt){
            ecs.view<RigidbodyComponent, PlayerTagComponent>(
                [&](Entity e, RigidbodyComponent& r, auto& tag){
                    if(res.input.isKeyDown(KEY_A)){
                        r.velocity.x -= 100.0f * dt;
                    }
                    if(res.input.isKeyDown(KEY_D)){
                        r.velocity.x += 100.0f * dt;
                    }
                    if(res.input.isKeyDown(KEY_W)){
                        r.velocity.y -= 100.0f * dt;
                    }
                    if(res.input.isKeyDown(KEY_S)){
                        r.velocity.y += 100.0f * dt;
                    }
                }
            );
        }
    );
    w.pipeline.onUpdate(moveSystem);
    w.pipeline.onUpdate(aabbCollideSystem);

    w.run();
}
