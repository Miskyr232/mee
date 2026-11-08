#pragma once

#include <mee/utils/vector.hpp>
#include <raylib.h>

struct TransformComponent{
    Vector2f pos = Vector2f::zero();
    Vector2f scale = Vector2f::one();
    float rotation = 0.0f;
};
