#pragma once

#include "mee/utils/resource-manager.hpp"
#include <raylib.h>
#include <string>
#include "mee/utils/vector.hpp"

struct TextureMetadata{
    Vector2i size;
};

using TextureID = uint32_t;

class TextureManager{
    ResourceManager<Texture, TextureID> textures;
public:
    TextureID loadTexture(const std::string path, bool smooth = false){
        return textures.load(path, [smooth](const char* path){
            Texture texture = LoadTexture(path);
            if(smooth){
                GenTextureMipmaps(&texture);
                SetTextureFilter(texture, TEXTURE_FILTER_TRILINEAR);
            }
            return texture;
        });
    }
    const Texture& getTexture(TextureID id){
        return textures.get(id);
    }
    ~TextureManager(){
        textures.clear([](Texture& s) { UnloadTexture(s);});
    }
    TextureMetadata getTextureMetadata(TextureID id){
        if(!textures.isValidId(id)){
            return {
                .size = Vector2i(0)
            };
        }
        const Texture& texture = textures.get(id);
        return {
            .size = Vector2i(texture.width, texture.height)
        };
    }
};
