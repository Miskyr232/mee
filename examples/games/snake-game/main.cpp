#include "mee/ecs/ecs.hpp"
#include "mee/core/world.hpp"
#include "mee/systems/render/render-sprite.hpp"
#include "mee/utils/color.hpp"
#include "mee/utils/vector.hpp"
#include "mee/engine-resources/engine-resources.hpp"
#include "mee/systems/render/camera-target.hpp"
#include "mee/systems/physics/aabb-collision.hpp"
#include "mee/systems/physics/move.hpp"
#include <filesystem>
#include <raylib.h>
#include <vector>

void moveSnake(std::vector<Vector2i>& snake, Vector2i dir, bool eaten){
    auto oldSnake = snake;
    snake[0] += dir;
    for(int i = 1; i < oldSnake.size(); i++){
        snake[i] = oldSnake[i - 1];
    }
    if(eaten){
        snake.push_back(oldSnake.back());
    }
}

bool checkLoose(const std::vector<Vector2i>& snake){
    for(int i = 2; i < snake.size(); i++){
        if(snake[0] == snake[i]){
            return true;
        }
    }
    return false;
}

int main(){
    Engine w;
    w.config.debug.enabled = true;
    w.config.debug.showColliders = true;
    w.config.window.targetFPS = 60;
    w.init();
    std::vector<Vector2i> snake(2);
    std::vector<Vector2i> apples(10);
    float snakeSize = 50.0f;
    Vector2i dir;
    float time = 0.0f;
    float duration = 0.3f;
    bool eaten = false;

    w.pipeline.addStartSystem(
        [&](ECS& ecs, EngineResources& res){
            for(auto& apple : apples){
                apple.x = res.random.getRandomInt(-10, 10);
                apple.y = res.random.getRandomInt(-10, 10);
            }
        }
    );
    w.pipeline.onUpdate(
        [&](ECS& ecs, EngineResources &res, float dt){
            for(auto& apple : apples){
                if(apple == snake[0]){
                    apple.x = res.random.getRandomInt(snake[0].x - 10, snake[0].x + 10);
                    apple.y = res.random.getRandomInt(snake[0].y - 10, snake[0].y + 10);
                    eaten = true;
                }
            }
        }
    );
    w.pipeline.onUpdate(
        [&](ECS& ecs, EngineResources &res, float dt){
            if(res.input.isKeyDown(KEY_W) && dir.y != 1){
                dir.y = -1;
                dir.x = 0;
            }
            else if(res.input.isKeyDown(KEY_S) && dir.y != -1){
                dir.y = 1;
                dir.x = 0;
            }
            else if(res.input.isKeyDown(KEY_A) && dir.x != 1){
                dir.x = -1;
                dir.y = 0;
            }
            else if(res.input.isKeyDown(KEY_D) && dir.x != -1){
                dir.x = 1;
                dir.y = 0;
            }
            time -= dt;
            if(time <= 0.0f){
                time = duration;
                moveSnake(snake, dir, eaten);
                if(checkLoose(snake)){
                    w.exit();
                }
                if(eaten){
                    eaten = false;
                }
                res.camera.moveTo(Vector2f(snake[0].x * snakeSize, snake[0].y * snakeSize), duration);
            }
        }
    );
    w.pipeline.onDraw(
        [&](ECS& ecs, EngineResources& res){
            for(int i = 0; i < snake.size(); i++){
                float thinkness = 10.0f + ((snake.size() - i) / 2.0f);
                float offset = (snakeSize - thinkness) / 2.0f;
                if(i != 0 && i != snake.size() - 1){
                    auto after = snake[i + 1];
                    auto current = snake[i];
                    auto before = snake[i - 1];
                    auto da = after - current;
                    auto db = before - current;

                    if(da.x == 0 && db.x == 0){
                        res.renderer.renderRect(
                            {
                                Vector2f(current.x * snakeSize + offset, current.y * snakeSize),
                                Vector2f(thinkness, snakeSize),
                                Color4f(0.2f, 0.6f, 0.2f)
                            },
                            10, false
                        );
                    }
                    else if(da.y == 0 && db.y == 0){
                        res.renderer.renderRect(
                            {
                                Vector2f(current.x * snakeSize, current.y * snakeSize + offset),
                                Vector2f(snakeSize, thinkness),
                                Color4f(0.2f, 0.6f, 0.2f)
                            },
                            10, false
                        );
                    }else {
                        float cx = current.x * snakeSize + offset;
                        float cy = current.y * snakeSize + offset;

                        res.renderer.renderRect(
                            { Vector2f(cx, cy), Vector2f(thinkness, thinkness), Color4f(0.2f, 0.6f, 0.2f) },
                            10, false
                        );

                        auto drawArm = [&](Vector2i neighbor) {
                            if (neighbor.y < current.y) {
                                res.renderer.renderRect(
                                    { Vector2f(cx, current.y * snakeSize), Vector2f(thinkness, offset + thinkness), Color4f(0.2f, 0.6f, 0.2f) },
                                    10, false
                                );
                            } else if (neighbor.y > current.y) {
                                res.renderer.renderRect(
                                    { Vector2f(cx, cy), Vector2f(thinkness, offset + thinkness), Color4f(0.2f, 0.6f, 0.2f) },
                                    10, false
                                );
                            } else if (neighbor.x < current.x) {
                                res.renderer.renderRect(
                                    { Vector2f(current.x * snakeSize, cy), Vector2f(offset + thinkness, thinkness), Color4f(0.2f, 0.6f, 0.2f) },
                                    10, false
                                );
                            } else if (neighbor.x > current.x) {
                                res.renderer.renderRect(
                                    { Vector2f(cx, cy), Vector2f(offset + thinkness, thinkness), Color4f(0.2f, 0.6f, 0.2f) },
                                    10, false
                                );
                            }
                        };

                        drawArm(before);
                        drawArm(after);
                    }
                }else if(i == 0){
                    auto current = snake[i];
                    auto after = snake[i + 1];
                    if(current.x == after.x){
                        res.renderer.renderRect(
                            {
                                Vector2f(current.x * snakeSize + offset, current.y * snakeSize),
                                Vector2f(thinkness, snakeSize),
                                Color4f(0.2f, 0.6f, 0.2f)
                            },
                            10,
                            false
                        );
                    }else if(current.y == after.y){
                        res.renderer.renderRect(
                            {
                                Vector2f(current.x * snakeSize, current.y * snakeSize + offset),
                                Vector2f(snakeSize, thinkness),
                                Color4f(0.2f, 0.6f, 0.2f)
                            },
                            10,
                            false
                        );
                    }
                }else if(i == snake.size() - 1){
                    auto current = snake[i];
                    auto before = snake[i - 1];
                    if(current.x == before.x){
                        res.renderer.renderRect(
                            {
                                Vector2f(current.x * snakeSize + offset, current.y * snakeSize),
                                Vector2f(thinkness, snakeSize),
                                Color4f(0.2f, 0.6f, 0.2f)
                            },
                            10,
                            false
                        );
                    }else if(current.y == before.y){
                        res.renderer.renderRect(
                            {
                                Vector2f(current.x * snakeSize, current.y * snakeSize + offset),
                                Vector2f(snakeSize, thinkness),
                                Color4f(0.2f, 0.6f, 0.2f)
                            },
                            10,
                            false
                        );
                    }
                }
            }
            for(auto apple : apples){
                res.renderer.renderCircle(
                    {
                        Vector2f(apple.x * snakeSize + snakeSize / 2.0f, apple.y * snakeSize + snakeSize / 2.0f),
                        snakeSize / 5.0f,
                        Color4f::Red()
                    },
                    0,
                    false
                );
            }
        }
    );
    w.run();
}
