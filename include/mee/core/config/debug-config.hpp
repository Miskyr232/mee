#include "mee/utils/color.hpp"

struct DebugConfig{
    Color4f contrastColor = Color4f::White();
    bool enabled = false;

    bool showFPS = true;
    bool showColliders = false;
    bool showEntityCount = false;
    bool showProfiler = true;
    bool showRenderCalls = true;
};
