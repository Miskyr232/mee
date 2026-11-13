#include "mee/ecs/ecs.hpp"
#include "mee/core/engine.hpp"
#include "mee/systems/render/render-sprite.hpp"
#include "mee/utils/color.hpp"
#include "mee/utils/timer.hpp"
#include "mee/utils/vector.hpp"
#include "mee/engine-resources/engine-resources.hpp"
#include "mee/systems/render/camera-target.hpp"
#include "mee/systems/physics/aabb-collision.hpp"
#include "mee/systems/physics/move.hpp"
#include <cstdio>
#include <raylib.h>

struct GridPosComponent{
    Vector2i pos = {0, 0};
};

struct SnakeHeadComponent{
    Vector2i pendingDir = {1, 0};
    Vector2i dir = {1, 0};
    Timer moveTimer{0.3f};
};

struct SnakeSegmentComponent{};

struct FoodComponent{};

void addApple(ECS& ecs, EngineResources& res){
    bool spawned = false;
    while(!spawned){
        Vector2i pos = Vector2i(
            res.random.getRandomInt(-10, 10),
            res.random.getRandomInt(-10, 10));

        bool occupied = false;
        ecs.view<GridPosComponent>([&](Entity, GridPosComponent& c){
            if(c.pos == pos) occupied = true;
        });

        if(!occupied){
            ecs.createEntity(
                GridPosComponent{.pos = pos},
                FoodComponent{}
            );
            spawned = true;
        }
    }
}

void createSnake(ECS& ecs){
    ecs.createEntity(
        SnakeHeadComponent{},
        GridPosComponent{.pos = {0, 0}}
    );
    ecs.createEntity(
        SnakeSegmentComponent{},
        GridPosComponent{.pos = {-1, 0}}
    );
    ecs.createEntity(
        SnakeSegmentComponent{},
        GridPosComponent{.pos = {-2, 0}}
    );
}

int main(){
    Engine w;
    w.config.debug.enabled = true;
    w.config.debug.showColliders = true;
    w.config.window.targetFPS = 60;
    w.pipeline.onStart(
        [&](ECS& ecs, EngineResources& res){
            createSnake(ecs);
            for(int i = 0; i <= 5; i++){
                ecs.createEntity(
                    FoodComponent{},
                    GridPosComponent{.pos = {res.random.getRandomInt(-10, 10), res.random.getRandomInt(-10, 10)}}
                );
            }
        }
    );
    w.init();
    w.pipeline.onUpdate(
        [&](ECS& ecs, EngineResources& res, float dt){
            ecs.view<SnakeHeadComponent>(
                [&](Entity e, SnakeHeadComponent& head){
                    if(res.input.isKeyPressed(KEY_D)){
                        head.pendingDir = {1, 0};
                    }if(res.input.isKeyPressed(KEY_A)){
                        head.pendingDir = {-1, 0};
                    }if(res.input.isKeyPressed(KEY_W)){
                        head.pendingDir = {0, -1};
                    }if(res.input.isKeyPressed(KEY_S)){
                        head.pendingDir = {0, 1};
                    }
                    if(res.input.isKeyDown(KEY_E)){
                        res.camera.zoom *= 1.0f + dt;
                    }if(res.input.isKeyDown(KEY_Q)){
                        res.camera.zoom /= 1.0f + dt;
                    }
                }
            );
        }
    );
    w.pipeline.onUpdate(
        [&](ECS& ecs, EngineResources& res, float dt){
            ecs.view<SnakeHeadComponent, GridPosComponent>(
                [&](Entity e, SnakeHeadComponent& h, GridPosComponent& hp){
                    if(!h.moveTimer.update(dt))
                        return;
                    //step
                    if(h.pendingDir != -h.dir)
                        h.dir = h.pendingDir;
                    Vector2i newPos = hp.pos + h.dir;

                    bool died = false;
                    if(newPos.x < -10 || newPos.x > 10 ||
                        newPos.y < -10 || newPos.y > 10)
                        died = true;
                    if(!died)
                        ecs.view<GridPosComponent>(
                            [&](Entity e, GridPosComponent& c){
                                if(ecs.hasComponent<FoodComponent>(e))
                                    return;
                                if(c.pos == newPos && c.pos != hp.pos)
                                    died = true;
                            }
                        );
                    if(died){
                        ecs.view<SnakeHeadComponent>(
                            [&](Entity e, SnakeHeadComponent&){
                                ecs.removeEntity(e);
                            }
                        );
                        ecs.view<SnakeSegmentComponent>(
                            [&](Entity e, SnakeSegmentComponent&){
                                ecs.removeEntity(e);
                            }
                        );
                        createSnake(ecs);
                        return;
                    }
                    Vector2i prevPos = hp.pos;
                    hp.pos = newPos;

                    ecs.view<SnakeSegmentComponent, GridPosComponent>(
                        [&](Entity e, SnakeSegmentComponent&, GridPosComponent& pos){
                            Vector2i tmp = pos.pos;
                            pos.pos = prevPos;
                            prevPos = tmp;
                        }
                    );

                    bool grew = false;
                    ecs.view<FoodComponent, GridPosComponent>([&](Entity e, FoodComponent&, GridPosComponent& pos){
                        if(pos.pos == newPos){
                            grew = true;
                            ecs.removeEntity(e);
                            addApple(ecs, res);
                        }
                    });

                    if(grew)
                        ecs.createEntity(
                            SnakeSegmentComponent{},
                            GridPosComponent{prevPos}
                        );
                }
            );
        }
    );
    w.pipeline.onDraw(
        [&](ECS& ecs, EngineResources& res){
            ecs.view<GridPosComponent, SnakeSegmentComponent>(
                [&](Entity e, GridPosComponent& pos, SnakeSegmentComponent&){
                    res.renderer.renderRect(
                        {
                            pos.pos.tof() * 100.0f,
                            Vector2f(100.0f),
                            Color4f::Green()
                        }
                    );
                }
            );
            ecs.view<GridPosComponent, SnakeHeadComponent>(
                [&](Entity e, GridPosComponent& pos, SnakeHeadComponent&){
                    res.renderer.renderRect(
                        {
                            pos.pos.tof() * 100.0f,
                            Vector2f(100.0f),
                            Color4f(0.0f, 0.96f, 0.0f)
                        }
                    );
                }
            );
        }
    );
    w.pipeline.onDraw(
        [&](ECS& ecs, EngineResources& res){
            ecs.view<GridPosComponent, FoodComponent>(
                [&](Entity e, GridPosComponent& pos, FoodComponent&){
                    res.renderer.renderRect(
                        {
                            pos.pos.tof() * 100.0f,
                            Vector2f(100.0f),
                            Color4f::Red()
                        }
                    );
                }
            );
        }
    );
    w.run();
}
