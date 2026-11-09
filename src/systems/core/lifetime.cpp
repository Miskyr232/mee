#include <mee/systems/core/lifetime.hpp>
#include <mee/components/core/lifetime.hpp>
#include <mee/ecs/ecs.hpp>

void lifetimeSystem(ECS& ecs, EngineResources& res, float dt){
    std::vector<Entity> toDelete;
    ecs.view<LifetimeCompnent>([&](Entity e, LifetimeCompnent l){
        l.elapsed += dt;
        if(l.elapsed >= l.lifetime){
            toDelete.push_back(e);
        }
    });
    for(Entity e : toDelete){
        ecs.removeEntity(e);
    }
}
