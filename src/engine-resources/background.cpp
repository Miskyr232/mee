#include "mee/engine-resources/engine-resources.hpp"
#include "mee/utils/color.hpp"
#include "mee/utils/vector.hpp"
#include <cmath>
#include <mee/engine-resources/background.hpp>

void Background::renderBackground(EngineResources& res) {
    const auto metadata =
        res.textureManager.getTextureMetadata(texture);

    const float zoom = res.camera.zoom;
    const float tileW = metadata.size.x * scale;
    const float tileH = metadata.size.y * scale;

    if (zoom <= 0.0f || tileW <= 0.0f || tileH <= 0.0f)
        return;

    const float viewW = GetScreenWidth() / zoom;
    const float viewH = GetScreenHeight() / zoom;

    Vector2f viewTL = res.camera.screenToWorld({0.0f, 0.0f});
    viewTL.x = std::floor(viewTL.x / tileW) * tileW;
    viewTL.y = std::floor(viewTL.y / tileH) * tileH;

    const float quadW = viewW + tileW * 2.0f;
    const float quadH = viewH + tileH * 2.0f;

    res.renderer.renderTexture(
        {
            texture,
            viewTL,
            scale,
            {0.0f, 0.0f},
            res.camera.rotation,
            Color4f::White(),
            {quadW / tileW, quadH / tileH}
        }
    );
}
