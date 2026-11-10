#pragma once

#include "mee/ecs/ecs.hpp"
#include <mee/core/pipeline.hpp>
#include "mee/core/config/engine-config.hpp"
#include "mee/engine-resources/engine-resources.hpp"
#include "mee/render/texture-manager.hpp"

class Engine{
    bool runing = true;
    TextureID colliderTextureID;
public:
    EngineConfig config;
    ECS ecs;
    Pipeline pipeline;
    EngineResources res;

    ~Engine();
    void run();

    void init();
    void renderColliders();
    void exit();
};
