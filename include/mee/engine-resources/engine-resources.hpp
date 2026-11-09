#pragma once

#include <mee/render/texture-manager.hpp>
#include "input-state.hpp"
#include <mee/engine-resources/camera.hpp>
#include <mee/render/background.hpp>
#include <mee/render/renderer.hpp>
#include <mee/render/particle-manager.hpp>
#include <mee/core/signal.hpp>
#include <mee/ecs/ecs.hpp>
#include <mee/sound/sound-manager.hpp>
#include <mee/utils/random.hpp>

struct EngineResources{
    TextureManager textureManager;
    SoundManager soundManager;
    InputState input;
    Random random;
    WorldCamera camera;
    BackgroundSettings background;
    ParticleManager particleManager;
    Renderer renderer;
    Signal<Entity, Entity> collisionSignal;
    Signal<> engineEnd;
};
