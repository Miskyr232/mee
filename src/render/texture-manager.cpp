#include <mee/render/texture-manager.hpp>
#include <cassert>
#include <raylib.h>

TextureID TextureManager::loadTexture(const std::string& path){
    auto it = pathToID.find(path);
    if(it != pathToID.end()){
        TextureID id = pathToID[path];
        return id;
    }
    TextureID id = next++;

    pathToID[path] = id;
    textures[id] = LoadTexture(path.c_str());

    return id;
}

const Texture2D& TextureManager::getTexture(TextureID id) const{
    return textures.at(id);
}

TextureManager::~TextureManager(){
    for(auto& pair : textures){
        UnloadTexture(pair.second);
    }
    textures.clear();
}
