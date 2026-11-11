#include <mee/render/texture-manager.hpp>
#include <cassert>
#include <raylib.h>

TextureID TextureManager::loadTexture(const std::string& path) {
    if(auto it = pathToID.find(path); it != pathToID.end())
        return it->second;

    TextureID id = textures.size();

    pathToID[path] = id;
    textures.push_back(LoadTexture(path.c_str()));

    return id;
}

const Texture2D& TextureManager::getTexture(TextureID id) const{
    return textures.at(id);
}

TextureManager::~TextureManager(){
    for(auto& texture : textures){
        UnloadTexture(texture);
    }
    textures.clear();
}

TextureMatadata TextureManager::getTextureMetadata(TextureID id) const{
    Texture2D texture = textures.at(id);
    return {
        Vector2i(texture.width, texture.height)
    };
}
