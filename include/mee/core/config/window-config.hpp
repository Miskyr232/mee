#include "mee/utils/color.hpp"
#include <string>

struct WindowConfig{
    int width = 1200;
    int height = 800;
    std::string title = "window";

    bool resizable = true;
    bool vsync = true;
    bool fullscreen = false;

    int targetFPS = 60;
    Color4f backgroundColor = Color4f::White();
};
