#pragma once

#include "mee/utils/resource-manager.hpp"
#include <raylib.h>
#include <string>

using SoundID = size_t;

class SoundManager{
    ResourceManager<Sound, SoundID> sounds;
public:
    SoundID loadSound(const std::string path){
        return sounds.load(path, [](const char* path){return LoadSound(path);});
    }
    const Sound& getSound(SoundID id){
        return sounds.get(id);
    }
    ~SoundManager(){
        sounds.clear([](Sound& sound){UnloadSound(sound);});
    }
};
