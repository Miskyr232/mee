#include <mee/systems/core/lifetime.hpp>
#include <mee/components/core/lifetime.hpp>
#include <mee/ecs/ecs.hpp>

void lifetimeSystem(ECS& ecs, EngineResources& res, float dt){
    ecs.view<LifetimeCompnent>([&](Entity e, LifetimeCompnent& l){
        l.elapsed += dt;
        if(l.elapsed >= l.lifetime){
            ecs.removeEntity(e);
        }
    });
}
