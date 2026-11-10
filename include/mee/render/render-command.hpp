#pragma once

#include <mee/render/texture-manager.hpp>
#include <mee/utils/vector.hpp>
#include <mee/utils/color.hpp>
#include <variant>

struct TextureCmd{
    TextureID texture;
    Vector2f pos;
    Vector2f scale = {1.0f, 1.0f};
    Vector2f origin = {0.5f, 0.5f};
    float rotation = 0.0f;
    Color4f tint = Color4f::White();
    Vector2f tileSize = {1.0f, 1.0f};
};

struct CircleCmd{
    Vector2f center;
    float radius;
    Color4f color;
};

struct RingCmd{
    Vector2f center;
    float inRadius;
    float outRadius;
    Color4f color;
};

struct TextCmd{
    std::string text;
    Vector2f position;
    float fontSize = 20.0f;
    Color4f color = Color4f::Black();
};

struct RectCmd{
    Vector2f positon;
    Vector2f size;
    Color4f color;
};

struct LineCmd{
    Vector2f start;
    Vector2f end;
    Color4f color;
    float thickness = 2.0f;
};

struct RenderCmd{
    float zIndex = 0.0f;
    bool isUI = false;
    std::variant<TextureCmd, CircleCmd, TextCmd, RingCmd, RectCmd, LineCmd> variant;
};
