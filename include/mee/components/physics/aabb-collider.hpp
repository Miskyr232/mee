#pragma once

#include <mee/utils/vector.hpp>

struct AABBColliderComponent{
    Vector2f offset = Vector2f::zero();
    Vector2f halfSize = Vector2f::one() * 100;
};
