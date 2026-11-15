#pragma once

#include "mee/utils/resource-manager.hpp"
#include <raylib.h>
#include <string>
#include "mee/utils/vector.hpp"

struct TextureMetadata{
    Vector2i size;
};

using TextureID = size_t;

class TextureManager{
    ResourceManager<Texture, TextureID> textures;
public:
    TextureID loadTexture(const std::string path){
        return textures.load(path, [](const char* path){return LoadTexture(path);});
    }
    const Texture& getTexture(TextureID id){
        return textures.get(id);
    }
    ~TextureManager(){
        textures.clear([](Texture& s) { UnloadTexture(s);});
    }
    TextureMetadata getTextureMetadata(TextureID id){
        const Texture& texture = textures.get(id);
        return {
            .size = Vector2i(texture.width, texture.height)
        };
    }
};
