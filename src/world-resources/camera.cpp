#include <mee/world-resources/camera.hpp>
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
