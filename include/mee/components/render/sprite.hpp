#pragma once

#include <mee/render/texture-manager.hpp>
#include <mee/utils/vector.hpp>

struct SpriteComponent{
    TextureID texture;
    Vector2f offset;
    float zIndex;

    bool flipX = false;
    bool flipY = false;

    bool repeatX = false;
    bool repeatY = false;
};
