#include <mee/ecs/ecs.hpp>
#include <cassert>
#include <iostream>

static void test_ecs(){
    ECS ecs;
    Entity a = ecs.createEntity();
    Entity b = ecs.createEntity();
    assert(a.index != b.index);
    ecs.removeEntity(a);
    assert(!ecs.isAlive(a));
    assert(ecs.isAlive(b));
    Entity c = ecs.createEntity();
    assert(a.index == c.index);
    std::cout << "full test passed" << std::endl;
}

int main(){
    test_ecs();
}
