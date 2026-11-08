#pragma once

#include <mee/utils/vector.hpp>

struct CircleColliderComponent{
    float radius = 100.0f;
    Vector2d offset = Vector2d::zero();
};
