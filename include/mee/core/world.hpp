#pragma once

#include "mee/ecs/ecs.hpp"
#include <mee/core/pipeline.hpp>
#include <mee/engine-resources/engine-resources.hpp>
#include "mee/core/config/engine-config.hpp"

class Engine{
public:
    EngineConfig config;
    ECS ecs;
    Pipeline pipeline;
    EngineResources res;

    ~Engine();
    void run();

    void init();
    void renderColliders();
};
