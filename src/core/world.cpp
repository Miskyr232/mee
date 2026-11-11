#include "mee/utils/color.hpp"
#include <mee/core/world.hpp>
#include <mee/components/core/transform.hpp>
#include <mee/components/physics/aabb-collider.hpp>
#include <mee/utils/vector.hpp>
#include <mee/engine-resources/engine-resources.hpp>
#include <mee/utils/profiler.hpp>
#include <raylib.h>

void Engine::init(){
    if(config.window.resizable)
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitAudioDevice();
    InitWindow(config.window.width, config.window.height, config.window.title.c_str());
    SetExitKey(KEY_NULL);
    SetTargetFPS(config.window.targetFPS);

    if(config.window.fullscreen)
        ToggleFullscreen();

    colliderTextureID = res.textureManager.loadTexture("assets/collider.png");

    printProfiler.setDuration(1.0f);

    pipeline.runStart(ecs, res);
    res.renderer.init();
}

Engine::~Engine(){
    res.engineEnd.emit();
    CloseAudioDevice();
    CloseWindow();
}

void Engine::renderColliders() {
    ecs.view<AABBColliderComponent, TransformComponent>(
        [&](Entity e, AABBColliderComponent& aabb, TransformComponent& transform) {
            res.renderer.renderTexture(
                {
                    colliderTextureID,
                    transform.pos,
                    aabb.halfSize / 128.0f * transform.scale * 2.0f,
                    Vector2f(0.5f, 0.5f),
                    0.0f,
                    Color4f(1.0f, 1.0f, 1.0f, 0.2f)
                },
                100
            );
        }
    );
}

void Engine::exit(){
    runing = false;
}

void Engine::render(){
    BeginDrawing();
    res.renderer.beginFrame();

    ClearBackground(config.window.backgroundColor.toRaylib());
    pipeline.runDraw(ecs, res);
    res.particleManager.draw(res);

    if(config.debug.enabled && config.debug.showColliders)
        renderColliders();
    res.renderer.flush(res.textureManager, res.camera);
    if(config.debug.enabled && config.debug.showFPS)
        DrawFPS(10, 10);
    EndDrawing();
}

void Engine::update(float dt){
    res.input.fill();
    pipeline.runUpdate(ecs, res, dt);
    res.particleManager.update(dt);
    res.camera.update(res, dt);
}

void Engine::run(){
    while(!WindowShouldClose()){
        if(!runing)
            break;
        float dt = GetFrameTime();

        Profiler::instance().profile("update", [&]{
            update(dt);
        });

        Profiler::instance().profile("render", [&]{
            render();
        });

        Profiler::instance().update(dt);
        if(printProfiler.update(dt)){
            Profiler::instance().printInfo();
        }
    }
}
