#pragma once

#include <mee/utils/vector.hpp>
#include <raylib.h>

class EngineResources;

class WorldCamera{
    float amplitude = 0.0f;

    Vector2f startPos;
    Vector2f targetPos;
    float moveDuration = 0.0f;
    float moveTimer = 0.0f;
    bool isMoving = false;

    public:
    Vector2f pos = Vector2f::zero();
    Vector2f shakePos = Vector2f::zero();
    Vector2f size = Vector2f::one();
    float rotation = 0.0f;
    float zoom = 1.0f;

    Vector2f worldToScreen(Vector2f world);
    Vector2f screenToWorld(Vector2f screen);
    void update(EngineResources& res, float dt);
    void shake(float amplitude);
    void moveTo(Vector2f target, float duration);
};
