#pragma once

#include <mee/ecs/ecs.hpp>
#include <functional>
#include <vector>

struct WorldResources;

using UpdateSystem = std::function<void(ECS&, WorldResources&, float)>;
using StartSystem = std::function<void(ECS&, WorldResources&)>;
using DrawSystem = std::function<void(ECS&, WorldResources&)>;

class Pipeline{
private:
    std::vector<UpdateSystem> updateSystems;
    std::vector<StartSystem> startSystems;
    std::vector<DrawSystem> drawSystems;
public:
    void onDraw(DrawSystem system);
    void addStartSystem(StartSystem system);
    void onUpdate(UpdateSystem system);

    void runUpdate(ECS& ecs, WorldResources& res, float dt);
    void runStart(ECS& ecs, WorldResources& res);
    void runDraw(ECS& ecs, WorldResources& res);
};
