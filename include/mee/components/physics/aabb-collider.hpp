#pragma once

#include <mee/utils/vector.hpp>

struct AABBColliderComponent{
    Vector2d offset = Vector2d::zero();
    Vector2d halfSize = Vector2d::one() * 100;
};
