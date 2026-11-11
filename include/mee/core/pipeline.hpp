#pragma once

#include <mee/ecs/ecs.hpp>
#include <functional>
#include <vector>

struct EngineResources;

using UpdateSystem = std::function<void(ECS&, EngineResources&, float)>;
using StartSystem = std::function<void(ECS&, EngineResources&)>;
using DrawSystem = std::function<void(ECS&, EngineResources&)>;

class Pipeline{
private:
    std::vector<UpdateSystem> updateSystems;
    std::vector<StartSystem> startSystems;
    std::vector<DrawSystem> drawSystems;
public:
    void onDraw(DrawSystem system);
    void onStart(StartSystem system);
    void onUpdate(UpdateSystem system);

    void runUpdate(ECS& ecs, EngineResources& res, float dt);
    void runStart(ECS& ecs, EngineResources& res);
    void runDraw(ECS& ecs, EngineResources& res);
};
