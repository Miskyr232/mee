#pragma once

#include <cstdint>
#include <raylib.h>
#include <unordered_map>
#include <string>

using TextureID = uint32_t;

class TextureManager{
    std::unordered_map<std::string, TextureID> pathToID;
    std::unordered_map<TextureID, Texture2D> textures;
    TextureID next = 0;

    public:
    TextureID loadTexture(const std::string& path);
    const Texture2D& getTexture(TextureID id) const;
    ~TextureManager();
};
