#include <mee/world-resources/input-state.hpp>
#include <mee/utils/vector.hpp>
#include <raylib.h>

void InputState::fill(){
    prev = curr;
    for(int k = 0; k < 512; k++)
        curr[k] = IsKeyDown(k);
}

bool InputState::isKeyDown(int k) {return curr[k];}
bool InputState::isKeyUp(int k) {return !curr[k];}
bool InputState::isKeyPressed(int k) {return curr[k] && !prev[k];}
bool InputState::isKeyReleased(int k) {return !curr[k] && prev[k];}

Vector2f InputState::getMousePos(){
    return GetMousePosition();
}
