#pragma once

#include <mee/utils/vector.hpp>
#include <mee/utils/sparse-set.hpp>
#include <mee/render/texture-manager.hpp>
#include <cstddef>
#include <raylib.h>
#include <vector>
#include <functional>

class ParticleEmitter;

struct EngineResources;

struct EmitterHandle{
    size_t index;
    size_t generation;
};

struct ParticleEmitterTransform {
    Vector2f position;
    Vector2f velocity;
    float rotation;
};

struct ParticleEmitterSettings {
    ParticleEmitterTransform transform{};
    float spread = 45.0f;
    float speed = 300.0f;
    float speedRandom = 0.2f;
    float scale = 10.0f;
    Vector2f textureScale = Vector2f::one();
    float sizeRandom = 0.2f;
    float lifetime = 10.0f;
    Color color = WHITE;
    float colorRandom = 0.0f;
    TextureID textureID;
    float gravity = 0.0f;
    float drag = 10.0f;
    int count = 10;
};

struct Particle{
    Vector2f pos;
    Vector2f vel;
    float scale;
    Vector2f textureScale;
    float lifeTime;
    float maxLifeTime;
    Color color;
    Texture texture;
    float rotation;
    bool fade;
    float gravity;
    float drag;
    EmitterHandle handle;
};

class ParticleEmitter{
    public:
    std::function<void(Particle&, float)> onUpdate;
    std::function<void(const Particle&, EngineResources&)> onDraw;
    ParticleEmitterSettings settings;
};

class ParticleManager{
    std::vector<Particle> particles;
    public:
    SparseSet<ParticleEmitter, EmitterHandle> emitters;
    void emit(EmitterHandle handle, EngineResources& res);
    void update(float dt);
    void draw(EngineResources& res);
    EmitterHandle createEmitter(const ParticleEmitterSettings& settings);
};

