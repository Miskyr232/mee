#include <mee/core/world.hpp>
#include <mee/components/core/transform.hpp>
#include <mee/components/physics/aabb-collider.hpp>
#include <mee/utils/vector.hpp>
#include <mee/world-resources/world-resources.hpp>
#include <raylib.h>

void renderBackgroundSystem(WorldResources& res) {
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
World::World(){
    InitAudioDevice();
    InitWindow(settings.width, settings.height, settings.title.c_str());
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
}

World::~World(){
    res.engineEnd.emit();
    CloseAudioDevice();
    CloseWindow();
}

void World::renderColliders() {
    ecs.view<AABBColliderComponent, TransformComponent>(
        [&](Entity e) {
            auto& aabb = ecs.getComponent<AABBColliderComponent>(e);
            auto& transform = ecs.getComponent<TransformComponent>(e);
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

void World::run(){
    pipeline.runStart(ecs, res);
    res.background.texture = res.textureManager.loadTexture("game/assets/space.png");
    res.renderer.init();
    while(!WindowShouldClose()){
        res.input.fill();
        pipeline.runUpdate(ecs, res, GetFrameTime());
        res.particleManager.update(GetFrameTime());

        BeginDrawing();
        res.renderer.beginFrame();

        ClearBackground(settings.backgroundColor.toRaylib());
        pipeline.runDraw(ecs, res);
        res.particleManager.draw(res);
        DrawFPS(10, 10);

        res.renderer.flush(res.textureManager, res.camera);
        EndDrawing();
    }
}
