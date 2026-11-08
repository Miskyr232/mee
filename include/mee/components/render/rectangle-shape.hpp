#pragma once

#include <raylib.h>

#include <mee/utils/vector.hpp>

struct RectangleShapeComponent{
    Vector2d offset = Vector2d::zero();
    Vector2d halfSize = Vector2d::one() * 50.0;
    Color color = WHITE;
};
