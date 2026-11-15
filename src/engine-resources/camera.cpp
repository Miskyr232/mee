#include "mee/engine-resources/engine-resources.hpp"
#include <algorithm>
#include <mee/engine-resources/camera.hpp>
#include <mee/utils/vector.hpp>
#include <mee/utils/math.hpp>
#include <raylib.h>

void WorldCamera::updateRotationCache(){
    if(rotation == cachedRotation)
        return;
    double angle = -(degToRadf(rotation));
    cachedCos = cos(angle);
    cachedSin = sin(angle);
    cachedRotation = rotation;
}

Vector2f WorldCamera::worldToScreen(Vector2f world) const{
    Vector2f relative = world - pos - shakePos;

    float rotatedX = relative.x * cachedCos - relative.y * cachedSin;
    float rotatedY = relative.x * cachedSin + relative.y * cachedCos;

    return {
        rotatedX * zoom + screenCenter.x,
        rotatedY * zoom + screenCenter.y
    };
}

Vector2f WorldCamera::screenToWorld(Vector2f screen) const{
    Vector2f relative = {
        (screen.x - screenCenter.x) / zoom,
        (screen.y - screenCenter.y) / zoom
    };

    // inverse rotation: cos stays the same, sin flips sign
    float rotatedX = relative.x * cachedCos + relative.y * cachedSin;
    float rotatedY = -relative.x * cachedSin + relative.y * cachedCos;

    return {
        rotatedX + pos.x + shakePos.x,
        rotatedY + pos.y + shakePos.y
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
    if(IsWindowResized()){
        screenCenter = Vector2f(GetScreenWidth(), GetScreenHeight()) / 2.0f;
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

void WorldCamera::setRotation(float targetRotation){
    rotation = targetRotation;
    updateRotationCache();
}
