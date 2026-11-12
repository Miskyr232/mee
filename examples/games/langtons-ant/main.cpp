#include "mee/ecs/ecs.hpp"
#include "mee/core/engine.hpp"
#include "mee/systems/render/render-sprite.hpp"
#include "mee/utils/color.hpp"
#include "mee/utils/vector.hpp"
#include "mee/engine-resources/engine-resources.hpp"
#include "mee/systems/render/camera-target.hpp"
#include "mee/systems/physics/aabb-collision.hpp"
#include "mee/systems/physics/move.hpp"
#include <cmath>
#include <map>
#include <raylib.h>
#include <string>

struct Vector2iCmp {
    bool operator()(const Vector2i& a, const Vector2i& b) const {
        if (a.x != b.x) return a.x < b.x;
        return a.y < b.y;
    }
};

int main(){
    Engine w;
    w.config.debug.enabled = true;
    w.config.debug.showColliders = false;
    w.config.window.resizable = true;
    w.config.window.fullscreen = true;
    w.init();

    Vector2i antPos{0, 0};
    int antDir = 0;

    std::map<Vector2i, uint8_t, Vector2iCmp> grid;

    float cellSize = 30.0f;
    float time = 0.0f;
    float stepInterval = 1.0f;
    float factor = 1;

    const Vector2i directions[4] = {
        Vector2i{0, -1}, // UP
        Vector2i{1, 0},  // RIGHT
        Vector2i{0, 1},  // DOWN
        Vector2i{-1, 0}  // LEFT
    };

    w.pipeline.onStart(
        [&](ECS& ecs, EngineResources& res){
            // for(int i = 0; i < 20; i++){
            //     int rx = res.random.getRandomInt(-5, 5);
            //     int ry = res.random.getRandomInt(-5, 5);
            //     grid[{rx, ry}] = 1;
            // }
        }
    );

    w.pipeline.onUpdate(
        [&](ECS& ecs, EngineResources &res, float dt){
            time -= dt;
            if(time <= 0.0f){
                time = stepInterval;

                bool isColored = grid.count(antPos) > 0 && grid[antPos] > 0;

                if(isColored){
                    antDir = (antDir + 3) % 4;
                    grid.erase(antPos);
                } else {
                    antDir = (antDir + 1) % 4;
                    grid[antPos] = 1;
                }
                antPos += directions[antDir];
            }
            if(res.input.isKeyDown(KEY_Q)){
                res.camera.zoom /= 1.05f;
            }
            if(res.input.isKeyDown(KEY_E)){
                res.camera.zoom *= 1.05f;
            }
            if(res.input.isKeyPressed(KEY_W) && stepInterval <= 100.0){
                stepInterval *= 2.0f;
            }
            if(res.input.isKeyPressed(KEY_S) && stepInterval >= 0.001){
                stepInterval /= 2.0f;
            }

            res.camera.moveTo(Vector2f(antPos.x * cellSize, antPos.y * cellSize), 0.1f);
        }
    );

    w.pipeline.onDraw(
        [&](ECS& ecs, EngineResources& res){
            for(const auto& [pos, _] : grid){
                res.renderer.renderRect(
                    {
                        Vector2f(pos.x * cellSize, pos.y * cellSize),
                        Vector2f(cellSize, cellSize),
                        Color4f::Black()
                   },
                    0, false
                );
            }

            res.renderer.renderRect(
                {
                    Vector2f(antPos.x * cellSize, antPos.y * cellSize),
                    Vector2f(cellSize, cellSize),
                    Color4f::Red()
                },
                0, false
            );
        }
    );

    w.run();
}
