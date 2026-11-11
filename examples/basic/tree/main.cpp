#include "mee/ecs/ecs.hpp"
#include "mee/core/world.hpp"
#include "mee/systems/render/render-sprite.hpp"
#include "mee/utils/color.hpp"
#include "mee/utils/vector.hpp"
#include "mee/engine-resources/engine-resources.hpp"
#include "mee/systems/render/camera-target.hpp"
#include "mee/systems/physics/aabb-collision.hpp"
#include "mee/systems/physics/move.hpp"
#include "mee/utils/profiler.hpp"
#include <cmath>
#include <raylib.h>
#include <string>

struct PlayerTagComponent{

};

Vector2f getLine(Vector2f start, float angle, float length){
    Vector2f dir(std::cosf(angle), std::sinf(angle));
    return start + dir * length;
}

void renderTree(
    EngineResources& res,
    Vector2f pos,
    float angle,
    int depth,
    float spread
){
    if(depth >= 15) return;

    float length = 100.0f * std::pow(0.7f, depth);

    Vector2f left = getLine(pos, angle - spread, length);
    Vector2f right = getLine(pos, angle + spread, length);

    res.renderer.renderLine({
        pos, left, Color4f::Red(), 0.2f
    });

    res.renderer.renderLine({
        pos, right, Color4f::Red(), 0.2f
    });

    renderTree(res, left, angle - spread, depth + 1, spread);
    renderTree(res, right, angle + spread, depth + 1, spread);
}

int main(){
    Engine w;
    w.config.debug.enabled = true;
    w.config.debug.showColliders = true;
    w.init();

    float angle = -3.14 / 2.0f;

    w.pipeline.onDraw(
        [&](ECS& ecs, EngineResources& res){
            Profiler::instance().profile("generate tree", [&]{
                renderTree(res, Vector2f(100, 100), angle, 0, 0.4);
            });
            res.renderer.renderText(
                {
                    std::to_string(angle),
                    res.camera.screenToWorld(Vector2f(0, 40)),
                    20 / res.camera.zoom,
                    Color4f::Black()
                }
            );
        }
    );
    w.pipeline.onUpdate(
        [&](ECS& ecs, EngineResources& res, float dt){
            if(res.input.isKeyDown(KEY_A)){
                res.camera.pos.x -= 40.0f * dt;
            }
            if(res.input.isKeyDown(KEY_D)){
                res.camera.pos.x += 40.0f * dt;
            }
            if(res.input.isKeyDown(KEY_W)){
                res.camera.pos.y -= 40.0f * dt;
            }
            if(res.input.isKeyDown(KEY_S)){
                res.camera.pos.y += 40.0f * dt;
            }
            if(res.input.isKeyDown(KEY_Q)){
                res.camera.zoom /= 1.01f;
            }
            if(res.input.isKeyDown(KEY_E)){
                res.camera.zoom *= 1.01f;
            }
        }
    );

    w.run();
}
