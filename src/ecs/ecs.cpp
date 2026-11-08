#include <mee/ecs/ecs.hpp>
#include <cassert>
#include <cstdint>

Entity ECS::createEntity(){
    if(!freeList.empty()){
        auto i = freeList.back();
        freeList.pop_back();
        slots[i].alive = true;
        return {i, slots[i].generation};
    }
    slots.push_back({0, true});
    return {static_cast<uint32_t>(slots.size() - 1), 0};
}

void ECS::removeEntity(Entity e){
    if(!isAlive(e) || e == Entity::Invalid())
        return;
    for(auto& [type, storage] : storages){
        storage->erase(e);
    }
    slots[e.index].alive = false;
    slots[e.index].generation++;
    freeList.push_back(e.index);
}

bool ECS::isAlive(Entity e) const {
    return e.index < slots.size()
        && slots[e.index].generation == e.generation
        && slots[e.index].alive;
}
