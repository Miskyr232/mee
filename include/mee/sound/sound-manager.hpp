#pragma once

#include <cstdint>
#include <raylib.h>
#include <unordered_map>
#include <string>

using SoundID = uint32_t;

class SoundManager{
    std::unordered_map<std::string, SoundID> pathToID;
    std::unordered_map<SoundID, Sound> sounds;
    SoundID next = 0;

    public:
    SoundID loadSound(const std::string& path);
    const Sound& getSound(SoundID id) const;
    ~SoundManager();
};
