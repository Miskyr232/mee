#include "mee/ecs/ecs.hpp"
#include "mee/core/engine.hpp"
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
#include <vector>

static float leftK = 1.5f;
static float rightK = 1.5f;

static std::vector<LineCmd> lines;

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
    float depth,
    float spread
){
    if(depth >= 10){
        return;
    }

    float length = 100.0f * std::pow(0.7f, depth);

    Vector2f left = getLine(pos, angle - spread, length);
    Vector2f right = getLine(pos, angle + spread, length);

    Color4f color = ((int)(depth * 10) % 2 == 0) ? Color4f::Black() : Color4f(0.8f, 0.8f, 0.0f);

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    lines.push_back({
        pos, left, color, 0.4f
    });
    lines.push_back({
        pos, right, color, 0.4f
    });

    renderTree(res, left, angle - spread, depth + leftK, spread);
    renderTree(res, right, angle + spread, depth + rightK, spread);
    renderTree(res, left, angle, depth + rightK * leftK, spread);
}

void updateTree(
    EngineResources& res,
    Vector2f pos,
    float angle,
    float depth,
    float spread
){
    lines.clear();
    Profiler::instance().measure("generate tree", [&]{
        renderTree(res, pos, angle, depth, spread);
    });
}

int main(){
    Engine w;
    w.config.debug.enabled = true;
    w.config.debug.showColliders = true;
    w.config.window.targetFPS = 60;
    w.init();

    float angle = -3.14 / 2.0f;
    float spread = 0.4;

    w.pipeline.onStart(
        [&](ECS& ecs, EngineResources& res){
            updateTree(res, Vector2f(100, 100), angle, 1, spread);
        }
    );
    w.pipeline.onDraw(
        [&](ECS& ecs, EngineResources& res){
            for(auto& cmd : lines){
                res.renderer.renderLine(cmd);
            }
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
            if(res.input.isKeyDown(KEY_R)){
                leftK += 0.01f * dt / 0.016f;
                rightK -= 0.01f * dt / 0.016f;
                updateTree(res, Vector2f(100, 100), angle, 1, spread);
            }
            if(res.input.isKeyDown(KEY_T)){
                leftK -= 0.01f * dt / 0.016f;
                rightK += 0.01f * dt / 0.016f;
                updateTree(res, Vector2f(100, 100), angle, 1, spread);
            }
            if(res.input.isKeyDown(KEY_Z)){
                spread += 0.01 * dt / 0.016f;
                updateTree(res, Vector2f(100, 100), angle, 1, spread);
            }
            if(res.input.isKeyDown(KEY_X)){
                spread -= 0.01f * dt / 0.016f;
                updateTree(res, Vector2f(100, 100), angle, 1, spread);
            }
        }
    );

    w.run();
}
