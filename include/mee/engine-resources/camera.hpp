#pragma once

#include <mee/utils/vector.hpp>
#include <limits>
#include <raylib.h>

class EngineResources;

class WorldCamera{
    float amplitude = 0.0f;

    Vector2f startPos;
    Vector2f targetPos;
    Vector2f screenCenter;
    float moveDuration = 0.0f;
    float moveTimer = 0.0f;
    bool isMoving = false;

    double cachedCos = 1.0;
    double cachedSin = 0.0;
    float cachedRotation = std::numeric_limits<float>::quiet_NaN();
    float transformA;
    float transformB;
    Vector2f translation;

    void updateRotationCache();

    public:
    Vector2f pos = Vector2f::zero();
    Vector2f shakePos = Vector2f::zero();
    Vector2f size = Vector2f::one();
    float rotation = 0.0f;
    float zoom = 1.0f;

    Vector2f worldToScreen(Vector2f world) const;
    Vector2f screenToWorld(Vector2f screen) const;
    void update(EngineResources& res, float dt);
    void shake(float amplitude);
    void moveTo(Vector2f target, float duration);
    void setRotation(float targetRotation);
    void updateTransformCache();
};
