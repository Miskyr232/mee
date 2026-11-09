#include <mee/core/pipeline.hpp>

void Pipeline::runStart(ECS& ecs, WorldResources& res){
    for(auto& system : startSystems){
        system(ecs, res);
    }
}
void Pipeline::runUpdate(ECS& ecs, WorldResources& res, float dt){
    for(auto& system : updateSystems){
        system(ecs, res, dt);
    }
}
void Pipeline::runDraw(ECS& ecs, WorldResources& res){
    for(auto& system : drawSystems){
        system(ecs, res);
    }
}

void Pipeline::onDraw(DrawSystem system){
    drawSystems.push_back(system);
}
void Pipeline::addStartSystem(StartSystem system){
    startSystems.push_back(system);
}
void Pipeline::onUpdate(UpdateSystem system){
    updateSystems.push_back(system);
}
