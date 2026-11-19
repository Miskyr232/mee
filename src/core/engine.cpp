#include "mee/render/texture-manager.hpp"
#include "mee/utils/color.hpp"
#include <mee/core/engine.hpp>
#include <mee/components/core/transform.hpp>
#include <mee/components/physics/aabb-collider.hpp>
#include <mee/utils/vector.hpp>
#include <mee/engine-resources/engine-resources.hpp>
#include <mee/utils/profiler.hpp>
#include <mee/systems/physics/aabb-collision.hpp>
#include <raylib.h>
#include <string>

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

    pipeline.runStart(ecs, res);
    res.renderer.init();
    Profiler::instance().measureInterval = 0.1f;
}

Engine::~Engine(){
    res.engineEnd.emit();
    CloseAudioDevice();
    CloseWindow();
}

void Engine::exit(){
    running = false;
}

void Engine::render(float dt){
    BeginDrawing();
    res.renderer.beginFrame();

    ClearBackground(config.window.backgroundColor.toRaylib());
    res.background.renderBackground(res);
    pipeline.runDraw(ecs, res);
    res.particleManager.draw(res);

    if(config.debug.enabled && config.debug.showColliders)
        renderColliders();
    if(config.debug.enabled && config.debug.showProfiler)
        Profiler::instance().measure("render profiler", [&]{
            renderProfiler(dt);
        });
    if(config.debug.enabled && config.debug.showRenderCalls)
        renderRenderCallsAmount();
    res.renderer.flush(res.textureManager, res.camera);
    if(config.debug.enabled && config.debug.showFPS)
        DrawFPS(10, 10);
}

void Engine::endRender(){
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
        if(!running)
            break;
        float dt = GetFrameTime();

        Profiler::instance().measure("update", [&]{
            update(dt);
        });

        Profiler::instance().measure("render", [&]{
            render(dt);
        });

        Profiler::instance().measure("idle", [&]{
            endRender();
        });

        Profiler::instance().measure("update profiler", [&]{
            Profiler::instance().update(dt);
        });
        nextDebugRenderPosY = 25;
    }
}

void Engine::renderColliders() {
    ecs.view<AABBColliderComponent, TransformComponent>(
        [&](Entity e, AABBColliderComponent& aabb, TransformComponent& transform) {
            AABB box = getAABB(transform, aabb);
            Vector2f size = box.max - box.min;
            res.renderer.renderRect(
                {
                    transform.pos - size / 2.0f + aabb.offset,
                    size,
                    Color4f(0.8f, 0.0f, 0.0f, 0.5f)
                },
                10000
            );
        }
    );
}

void Engine::renderProfiler(float dt){
    const auto& data = Profiler::instance().getData();

    float threshold = 400.0f;

    float maxVal = 0.0f;
    for(const auto& pair : data){
        if(pair.second.average > maxVal){
            maxVal = pair.second.average;
        }
    }

    float lerpSpeed = 15.0f;

    for(const auto& pair : data){
        float targetSize = maxVal > 0.0f
            ? pair.second.average / maxVal * threshold
            : 0.0f;

        auto it = m_smoothedSizes.find(pair.first);
        if(it == m_smoothedSizes.end()){
            m_smoothedSizes[pair.first] = targetSize;
            it = m_smoothedSizes.find(pair.first);
        }
        it->second += (targetSize - it->second) * (lerpSpeed * dt);
        float size = it->second < 2.0f ? 2.0f : it->second;

        const std::string& text = "ms " + Text::padRight(Text::trimFloat(pair.second.average, 4), 8);
        float textSize = text.size() * 20 * 0.6;
        res.renderer.renderText(
            {
                text,
                Vector2f(10, nextDebugRenderPosY),
                20.0f,
                config.debug.contrastColor
            },
            20,
            true
        );
        res.renderer.renderRect(
            {
                Vector2f(10, nextDebugRenderPosY) + Vector2f(textSize, 0),
                Vector2f(size, 20),
                Color4f(1.0f, 0.0f, 0.0f, 0.8f)
            },
            20,
            true
        );
        res.renderer.renderText(
            {
                pair.first,
                Vector2f(10, nextDebugRenderPosY) + Vector2f(5.0f + size + textSize, 0),
                20.0f,
                config.debug.contrastColor
            },
            20,
            true
        );
        nextDebugRenderPosY += 20;
    }
}

void Engine::renderRenderCallsAmount(){
    int callsAmount = res.renderer.getRenderCallsAmount();
    res.renderer.renderText(
        {
            std::to_string(callsAmount),
            Vector2f(10, nextDebugRenderPosY),
            20,
            config.debug.contrastColor
        },
        20,
        true
    );
    nextDebugRenderPosY += 20;
}
