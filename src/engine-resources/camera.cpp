#include "mee/engine-resources/engine-resources.hpp"
#include <algorithm>
#include <mee/engine-resources/camera.hpp>
#include <mee/utils/vector.hpp>
#include <mee/utils/math.hpp>

Vector2f WorldCamera::worldToScreen(Vector2f world){
    Vector2f relative = world - pos - shakePos;
    double cosA = cos(-(degToRadf(rotation)));
    double sinA = sin(-(degToRadf(rotation)));

    double rotatedX = relative.x * cosA - relative.y * sinA;
    double rotatedY = relative.x * sinA + relative.y * cosA;

    return {
        static_cast<float>(rotatedX * zoom + GetScreenWidth() * 0.5),
        static_cast<float>(rotatedY * zoom + GetScreenHeight() * 0.5)
    };
}

Vector2f WorldCamera::screenToWorld(Vector2f screen){
    Vector2f relative = {
        (screen.x - GetScreenWidth() * 0.5f) / zoom,
        (screen.y - GetScreenHeight() * 0.5f) / zoom
    };

    double cosA = cos(degToRadf(rotation));
    double sinA = sin(degToRadf(rotation));

    double rotatedX = relative.x * cosA - relative.y * sinA;
    double rotatedY = relative.x * sinA + relative.y * cosA;

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
