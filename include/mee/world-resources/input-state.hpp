#pragma once

#include <array>
#include <mee/utils/vector.hpp>

class InputState{
    std::array<bool, 512> prev;
    std::array<bool, 512> curr;
public:
    void fill();
    void updateCameraPos(Vector2f pos);

    bool isKeyDown(int k);
    bool isKeyUp(int k);
    bool isKeyPressed(int k);
    bool isKeyReleased(int k);

    Vector2f getMousePos();
};
