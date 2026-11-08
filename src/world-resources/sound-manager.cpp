#include "world-resources/sound/sound-manager.hpp"
#include <cassert>
#include <raylib.h>

SoundID SoundManager::loadSound(const std::string& path){
    auto it = pathToID.find(path);
    if(it != pathToID.end()){
        SoundID id = pathToID[path];
        return id;
    }
    SoundID id = next++;

    pathToID[path] = id;
    sounds[id] = LoadSound(path.c_str());

    return id;
}

const Sound& SoundManager::getSound(SoundID id) const{
    return sounds.at(id);
}

SoundManager::~SoundManager(){
    for(auto& pair : sounds){
        UnloadSound(pair.second);
    }
    sounds.clear();
}
