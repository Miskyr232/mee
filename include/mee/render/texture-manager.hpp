
#pragma once
#include "mee/utils/vector.hpp"
#include <raylib.h>
#include <unordered_map>
#include <string>
#include <vector>

using TextureID = size_t;

struct TextureMatadata{
    Vector2i size;
};

class TextureManager{
    std::unordered_map<std::string, TextureID> pathToID;
    std::vector<Texture2D> textures;
    TextureID next = 0;

    public:
    TextureID loadTexture(const std::string& path);
    const Texture2D& getTexture(TextureID id) const;
    TextureMatadata getTextureMetadata(TextureID id) const;
    ~TextureManager();
};
