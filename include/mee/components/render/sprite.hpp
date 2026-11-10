#pragma once

#include <mee/render/texture-manager.hpp>
#include <mee/utils/vector.hpp>

struct SpriteComponent{
    TextureID texture;
    Vector2f offset;
    Vector2f scale = Vector2f(1.0f, 1.0f);
    Vector2f tileSize = Vector2f(1.0f, 1.0f);
    float zIndex;
};
