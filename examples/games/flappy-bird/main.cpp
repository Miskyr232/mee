#include "mee/components/core/transform.hpp"
#include "mee/components/physics/aabb-collider.hpp"
#include "mee/components/physics/rigidbody.hpp"
#include "mee/components/render/sprite.hpp"
#include "mee/core/engine.hpp"
#include "mee/ecs/ecs.hpp"
#include "mee/engine-resources/engine-resources.hpp"
#include "mee/systems/render/render-sprite.hpp"
#include "mee/systems/render/camera-target.hpp"
#include "mee/systems/physics/aabb-collision.hpp"
#include "mee/systems/physics/move.hpp"
#include "mee/systems/render/render-animation.hpp"
#include "mee/systems/render/play-animation.hpp"
#include "mee/utils/color.hpp"
#include "mee/components/render/animation.hpp"
#include "mee/utils/vector.hpp"

struct BirdComponent{};

struct PipeComponent{};

struct GravitableComponent{};

void createPipes(ECS& ecs, EngineResources& res){
    float offset = res.random.getRandomInt(-100, 100);
    ecs.createEntity(
        PipeComponent{},
        TransformComponent{
            .pos = Vector2f(506, 230 + offset)
        },
        RigidbodyComponent{
            .invMass = 0
        },
        SpriteComponent{
            .texture = res.textureManager.loadTexture("examples/games/flappy-bird/assets/pipe.png"),
        },
        AABBColliderComponent{
            .halfSize = Vector2f(26, 160)
        }
    );
    ecs.createEntity(
        PipeComponent{},
        TransformComponent{
            .pos = Vector2f(506, -230 + offset)
        },
        RigidbodyComponent{
            .invMass = 0
        },
        SpriteComponent{
            .texture = res.textureManager.loadTexture("examples/games/flappy-bird/assets/pipe.png"),
            .offset = Vector2f(0, -320),
            .tileSize = Vector2f(1, -1)
        },
        AABBColliderComponent{
            .halfSize = Vector2f(26, 160)
        }
    );
}

int main(){
    Engine w;
    w.config.window.targetFPS = 60;
    w.config.debug.enabled = true;
    w.config.debug.showFPS = true;
    w.config.debug.showProfiler = false;
    w.config.debug.contrastColor = Color4f::Black();
    w.res.camera.zoom = 2.0f;
    w.init();
    w.ecs.createEntity(
        BirdComponent{},
        TransformComponent{},
        RigidbodyComponent{},
        GravitableComponent{},
        AnimationComponent{
            .animation = {
                .textures = {
                    w.res.textureManager.loadTexture("examples/games/flappy-bird/assets/bird/1.png"),
                    w.res.textureManager.loadTexture("examples/games/flappy-bird/assets/bird/2.png"),
                    w.res.textureManager.loadTexture("examples/games/flappy-bird/assets/bird/3.png"),
                    w.res.textureManager.loadTexture("examples/games/flappy-bird/assets/bird/4.png")
                },
                .currentTextureIndex = 0,
                .nextFrameTimer = {
                    .duration = 0.1f
                },
                .mode = PlaybackMode::Loop,
            },
        },
        AABBColliderComponent{
            .halfSize = Vector2f(17, 12)
        }
    );
    createPipes(w.ecs, w.res);
    w.pipeline.onUpdate(
        [&](ECS& ecs, EngineResources& res, float dt){
            ecs.view<RigidbodyComponent, GravitableComponent>(
                [&](Entity e, RigidbodyComponent& rb, GravitableComponent& g){
                    rb.velocity.y += 980 * dt;
                }
            );
        }
    );
    w.pipeline.onUpdate(moveSystem);
    w.pipeline.onUpdate(
        [&](ECS& ecs, EngineResources& res, float dt){
            ecs.view<TransformComponent, BirdComponent>(
                [&](Entity e, TransformComponent& tr, BirdComponent& b){
                    if(tr.rotation < 90)
                        tr.rotation += 90 * dt;
                }
            );
        }
    );
    w.pipeline.onUpdate(
        [&](ECS& ecs, EngineResources& res, float dt){
            ecs.view<RigidbodyComponent, TransformComponent, AnimationComponent, BirdComponent>(
                [&](Entity e, RigidbodyComponent& rb, TransformComponent& tr, AnimationComponent& anim, BirdComponent& b){
                    if(res.input.isKeyPressed(KEY_E)){
                        anim.animation.mode = PlaybackMode::Once;
                    }
                    if(res.input.isKeyPressed(KEY_SPACE)){
                        rb.velocity.y = -450;
                        tr.rotation = -30;
                    }
                }
            );
        }
    );
    w.pipeline.onUpdate(
        [&](ECS& ecs, EngineResources& res, float dt){
            ecs.view<TransformComponent, PipeComponent>(
                [&](Entity e, TransformComponent& tr, PipeComponent& pipe){
                    tr.pos.x -= 200 * dt;
                }
            );
        }
    );
    w.pipeline.onUpdate(
        [&](ECS& ecs, EngineResources& res, float dt){
            ecs.view<TransformComponent, PipeComponent>(
                [&](Entity e, TransformComponent& tr, PipeComponent& pipe){
                    if(tr.pos.x < -506){
                        ecs.removeEntity(e);
                    }
                }
            );
        }
    );
    w.pipeline.onUpdate(
        [&](ECS& ecs, EngineResources& res, float dt){
            static Timer spawnPipeTimer = {.duration = 1.5f};
            if(spawnPipeTimer.update(dt)){
                createPipes(ecs, res);
            }
        }
    );
    w.res.collisionSignal.connect(
        [&](Entity a, Entity b){
            if((w.ecs.hasComponent<PipeComponent>(a) && w.ecs.hasComponent<BirdComponent>(b)) || (w.ecs.hasComponent<PipeComponent>(b) && w.ecs.hasComponent<BirdComponent>(a))){
                w.ecs.createEntity(
                    TransformComponent{},
                    SpriteComponent{
                        .texture = w.res.textureManager.loadTexture("examples/games/flappy-bird/assets/gameover.png"),
                        .zIndex = 10
                    }
                );
            }
        }
    );
    w.pipeline.onUpdate(aabbCollideSystem);
    w.pipeline.onUpdate(playAnimationSystem);
    w.pipeline.onDraw(renderSpriteSystem);
    w.pipeline.onDraw(renderAnimationSystem);
    w.pipeline.onDraw(
        [&](ECS& ecs, EngineResources& res){
            res.renderer.renderTexture(
                {
                    res.textureManager.loadTexture("examples/games/flappy-bird/assets/background.png"),
                    Vector2f(0, 0),
                    Vector2f(1.2f, 1.2f),
                    Vector2f(0.5f, 0.5f),
                    0.0f,
                    Color4f::White(),
                    Vector2f(5, 1)
                },
                -10
            );
        }
    );
    w.run();
}
