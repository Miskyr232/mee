#include <mee/core/world.hpp>
#include <mee/components/core/transform.hpp>
#include <mee/components/physics/aabb-collider.hpp>
#include <mee/utils/vector.hpp>
#include <mee/engine-resources/engine-resources.hpp>
#include <raylib.h>

void renderBackgroundSystem(EngineResources& res) {
    Texture2D texture =
        res.textureManager.getTexture(res.background.texture);

    float scale = res.background.scale * res.camera.zoom;

    float tileW = texture.width * scale;
    float tileH = texture.height * scale;

    if (tileW <= 0 || tileH <= 0) return;

    float cameraX = res.camera.pos.x + res.camera.shakePos.x;
    float cameraY = res.camera.pos.y + res.camera.shakePos.y;

    float offsetX = fmodf(cameraX * res.camera.zoom, tileW);
    float offsetY = fmodf(cameraY * res.camera.zoom, tileH);

    if (offsetX < 0) offsetX += tileW;
    if (offsetY < 0) offsetY += tileH;

    int columns = static_cast<int>(GetScreenWidth() / tileW) + 2;
    int rows = static_cast<int>(GetScreenHeight() / tileH) + 2;

    float startX = GetScreenWidth() * 0.5f - offsetX;
    float startY = GetScreenHeight() * 0.5f - offsetY;

    for (int y = -rows; y <= rows; ++y) {
        for (int x = -columns; x <= columns; ++x) {
            DrawTextureEx(
                texture,
                {startX + x * tileW, startY + y * tileH},
                0.0f,
                scale,
                WHITE
            );
        }
    }
}

void Engine::init(){
    if(config.window.resizable)
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitAudioDevice();
    InitWindow(config.window.width, config.window.height, config.window.title.c_str());
    SetExitKey(KEY_NULL);
    SetTargetFPS(config.window.targetFPS);

    if(config.window.fullscreen)
        ToggleFullscreen();
}

Engine::~Engine(){
    res.engineEnd.emit();
    CloseAudioDevice();
    CloseWindow();
}

void Engine::renderColliders() {
    ecs.view<AABBColliderComponent, TransformComponent>(
        [&](Entity e, AABBColliderComponent& aabb, TransformComponent& transform) {
            float zoom = res.camera.zoom;
            Vector2f center = res.camera.worldToScreen(
                transform.pos + aabb.offset.tof()
            );
            float width = aabb.halfSize.x * 2.0f * zoom * transform.scale.x;
            float height = aabb.halfSize.y * 2.0f * zoom * transform.scale.y;
            DrawRectangle(
                static_cast<int>(center.x - width * 0.5f),
                static_cast<int>(center.y - height * 0.5f),
                static_cast<int>(width),
                static_cast<int>(height),
                Color(255, 0, 0, 20)
            );
        }
    );
}

void Engine::exit(){
    runing = false;
}

void Engine::run(){
    pipeline.runStart(ecs, res);
    res.background.texture = res.textureManager.loadTexture("game/assets/space.png");
    res.renderer.init();
    while(!WindowShouldClose()){
        if(!runing){
            CloseWindow();
        }
        res.input.fill();
        pipeline.runUpdate(ecs, res, GetFrameTime());
        res.particleManager.update(GetFrameTime());
        res.camera.update(res, GetFrameTime());

        BeginDrawing();
        res.renderer.beginFrame();

        ClearBackground(config.window.backgroundColor.toRaylib());
        pipeline.runDraw(ecs, res);
        res.particleManager.draw(res);

        res.renderer.flush(res.textureManager, res.camera);
        if(config.debug.enabled && config.debug.showColliders)
            renderColliders();
        if(config.debug.enabled && config.debug.showFPS)
            DrawFPS(10, 10);
        EndDrawing();
    }
}
