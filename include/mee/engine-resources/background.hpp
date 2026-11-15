#pragma once

#include "mee/render/texture-manager.hpp"
#include "mee/utils/vector.hpp"
#include "mee/engine-resources/engine-resources.hpp"

class Background{
public:
    float scale = 1.0f;
    TextureID texture = -1;
    void renderBackground(EngineResources& res);
};
