#pragma once

#include <raylib.h>

struct Color4f{
    float r, g, b, a;
    constexpr Color4f() = default;
    constexpr Color4f(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {};
    Color toRaylib() const;
    static Color4f fromRaylib(Color color);

    static constexpr Color4f White()       { return {1.0f, 1.0f, 1.0f, 1.0f}; }
    static constexpr Color4f Black()       { return {0.0f, 0.0f, 0.0f, 1.0f}; }
    static constexpr Color4f Transparent() { return {0.0f, 0.0f, 0.0f, 0.0f}; }
    static constexpr Color4f Red()         { return {1.0f, 0.0f, 0.0f, 1.0f}; }
    static constexpr Color4f Green()       { return {0.0f, 1.0f, 0.0f, 1.0f}; }
    static constexpr Color4f Blue()        { return {0.0f, 0.0f, 1.0f, 1.0f}; }
    static constexpr Color4f Yellow()      { return {1.0f, 1.0f, 0.0f, 1.0f}; }
};
