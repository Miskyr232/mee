#include <cstddef>
#include <vector>
#include "mee/render/texture-manager.hpp"
#include "mee/utils/timer.hpp"

enum class PlaybackMode {
    None,
    Once,
    Loop,
    PingPong
};

struct Animation{
    std::vector<TextureID> textures;
    size_t currentTextureIndex = -1;
    Timer nextFrameTimer;
    PlaybackMode mode;
};

struct AnimationComponent{
    Animation animation;

    float zIndex;
    bool isUI;
};
