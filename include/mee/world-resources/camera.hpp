#pragma once

#include <mee/utils/vector.hpp>
#include <raylib.h>

class WorldCamera{
    public:
    Vector2f pos = Vector2f::zero();
    Vector2f shakePos = Vector2f::zero();
    Vector2f size = Vector2f::one();
    float rotation = 0.0f;
    float zoom = 1.0f;

    Vector2f worldToScreen(Vector2f world);
    Vector2f screenToWorld(Vector2f screen);
};
