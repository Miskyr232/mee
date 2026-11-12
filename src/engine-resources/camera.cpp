#include "mee/engine-resources/engine-resources.hpp"
#include <algorithm>
#include <mee/engine-resources/camera.hpp>
#include <mee/utils/vector.hpp>
#include <mee/utils/math.hpp>

void WorldCamera::updateRotationCache(){
    if(rotation == cachedRotation)
        return;
    double angle = -(degToRadf(rotation));
    cachedCos = cos(angle);
    cachedSin = sin(angle);
    cachedRotation = rotation;
}

Vector2f WorldCamera::worldToScreen(Vector2f world){
    updateRotationCache();

    Vector2f relative = world - pos - shakePos;

    double rotatedX = relative.x * cachedCos - relative.y * cachedSin;
    double rotatedY = relative.x * cachedSin + relative.y * cachedCos;

    return {
        static_cast<float>(rotatedX * zoom + GetScreenWidth() * 0.5),
        static_cast<float>(rotatedY * zoom + GetScreenHeight() * 0.5)
    };
}

Vector2f WorldCamera::screenToWorld(Vector2f screen){
    updateRotationCache();

    Vector2f relative = {
        (screen.x - GetScreenWidth() * 0.5f) / zoom,
        (screen.y - GetScreenHeight() * 0.5f) / zoom
    };

    // inverse rotation: cos stays the same, sin flips sign
    double rotatedX = relative.x * cachedCos + relative.y * cachedSin;
    double rotatedY = -relative.x * cachedSin + relative.y * cachedCos;

    return {
        static_cast<float>(rotatedX + pos.x + shakePos.x),
        static_cast<float>(rotatedY + pos.y + shakePos.y)
    };
}

void WorldCamera::shake(float amplitude_){
    amplitude = amplitude_;
}

void WorldCamera::update(EngineResources& res, float dt){
    if(amplitude > 0.05){
        Vector2f randomDir = {
            res.random.getRandomInt(-100, 100) / 100.0f,
            res.random.getRandomInt(-100, 100) / 100.0f
        };
        shakePos = randomDir * amplitude;
        amplitude -= 10.0f * dt;
    } else{
        amplitude = 0.0f;
        shakePos = {0.0, 0.0};
    }

    if(isMoving){
        moveTimer += dt;
        float t = std::clamp(moveTimer / moveDuration, 0.0f, 1.0f);

        pos = startPos + (targetPos - startPos) * t;

        if(moveTimer >= moveDuration){
            pos = targetPos;
            isMoving = false;
        }
    }
}

void WorldCamera::moveTo(Vector2f target, float duration){
    startPos = pos;
    targetPos = target;
    moveDuration = duration;
    moveTimer = 0.0f;
    isMoving = duration > 0.0f;
    if(!isMoving){
        pos = targetPos;
    }
}
