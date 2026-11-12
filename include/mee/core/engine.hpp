#pragma once

#include "mee/ecs/ecs.hpp"
#include <mee/core/pipeline.hpp>
#include "mee/core/config/engine-config.hpp"
#include "mee/engine-resources/engine-resources.hpp"
#include "mee/render/texture-manager.hpp"
#include "mee/utils/timer.hpp"

class Engine{
    bool running = true;
    TextureID colliderTextureID;
    void render();
    void update(float dt);
    Timer printProfiler;
public:
    EngineConfig config;
    ECS ecs;
    Pipeline pipeline;
    EngineResources res;

    ~Engine();
    void run();
    void endRender();

    void init();
    void renderColliders();
    void exit();
};
