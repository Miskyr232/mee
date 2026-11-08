#pragma once

#include "mee/ecs/ecs.hpp"
#include <mee/core/pipeline.hpp>
#include <mee/world-resources/world-resources.hpp>

struct WorldSettings{
    int width = 1200;
    int height = 800;
    std::string title = "window";
    Color4f backgroundColor = Color4f::White();
};

class World{
    WorldSettings settings;
public:
    ECS ecs;
    Pipeline pipeline;
    WorldResources res;

    World();
    ~World();
    void run();
    void renderColliders();
};
