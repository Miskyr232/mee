#include <mee/render/particle-manager.hpp>

#include <mee/utils/vector.hpp>
#include <mee/world-resources/world-resources.hpp>
#include <algorithm>
#include <raylib.h>

void ParticleManager::update(float dt){
    for (size_t i = 0; i < particles.size();){  // delete died
        Particle& particle = particles[i];
        particle.lifeTime -= dt;
        ParticleEmitter* emitter = emitters.get(particle.handle);
        if (particle.lifeTime <= 0.0f || emitter == nullptr){
            particle = particles.back();
            particles.pop_back();
            continue;
        }
        if(emitter->onUpdate){
            emitter->onUpdate(particle, dt);
        }
        particle.vel.y += particle.gravity * dt;
        particle.vel *= (1.0f - particle.drag * dt);
        particle.pos += particle.vel * dt;
        ++i;
    }
}

void ParticleManager::draw(WorldResources& res){
    for(auto& particle : particles){
        ParticleEmitter* emitter = emitters.get(particle.handle);
        if (emitter && emitter->onDraw) {
            emitter->onDraw(particle, res);
        }else {
            Vector2 screenPos = res.camera.worldToScreen({particle.pos.x, particle.pos.y}).toRaylib();
            Texture& texture = particle.texture;
            Rectangle source = {
                0.0f, 0.0f,
                (float)texture.width,
                (float)texture.height
            };

            float zoom = res.camera.zoom;
            Rectangle dest = {
                screenPos.x, screenPos.y,
                zoom * particle.textureScale.x * particle.scale,
                zoom * particle.textureScale.y * particle.scale
            };

            Vector2 origin = {
                zoom * particle.textureScale.x * particle.scale * 0.5f,
                zoom * particle.textureScale.y * particle.scale * 0.5f
            };

            DrawTexturePro(
                texture,
                source,
                dest,
                origin,
                particle.rotation,
                particle.color
            );
        }
    }
}

void ParticleManager::emit(EmitterHandle handle, WorldResources& res) {
    ParticleEmitter* emitter = emitters.get(handle);
    const auto& settings = emitter->settings;
    for (int i = 0; i < settings.count; i++){
        float sizeRand   = GetRandomValue(-1000, 1000) / 1000.0f;
        float speedRand  = GetRandomValue(-1000, 1000) / 1000.0f;
        float spreadRand = GetRandomValue(-1000, 1000) / 1000.0f;

        float angle = (settings.transform.rotation + spreadRand * settings.spread) * DEG2RAD;
        float speedMul = 1.0f + speedRand * settings.speedRandom;

        Texture particleTexture = res.textureManager.getTexture(settings.textureID);

        Color color = settings.color;

        float r_offset = ((GetRandomValue(0, 200) - 100) / 100.0f) * settings.colorRandom;
        float g_offset = ((GetRandomValue(0, 200) - 100) / 100.0f) * settings.colorRandom;
        float b_offset = ((GetRandomValue(0, 200) - 100) / 100.0f) * settings.colorRandom;

        color.r = (unsigned char)std::clamp(color.r + (color.r * r_offset), 0.0f, 255.0f);
        color.g = (unsigned char)std::clamp(color.g + (color.g * g_offset), 0.0f, 255.0f);
        color.b = (unsigned char)std::clamp(color.b + (color.b * b_offset), 0.0f, 255.0f);

        Vector2f vel = {
            cosf(angle) * settings.speed * speedMul,
            sinf(angle) * settings.speed * speedMul
        };

        particles.push_back({
            .pos = settings.transform.position,
            .vel = settings.transform.velocity + vel,
            .scale = settings.scale * (1.0f + sizeRand * settings.sizeRandom),
            .textureScale = settings.textureScale,
            .lifeTime = settings.lifetime,
            .maxLifeTime = settings.lifetime,
            .color = color,
            .texture = particleTexture,
            .rotation = settings.transform.rotation,
            .gravity = settings.gravity,
            .drag = settings.drag,
            .handle = handle
        });
    }
}

EmitterHandle ParticleManager::createEmitter(const ParticleEmitterSettings& settings){
    EmitterHandle handle = emitters.create();
    ParticleEmitter* emitter = emitters.get(handle);
    emitter->settings = settings;
    return handle;
}

