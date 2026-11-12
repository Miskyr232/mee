#include "mee/components/core/transform.hpp"
#include "mee/components/physics/aabb-collider.hpp"
#include "mee/components/physics/rigidbody.hpp"
#include <mee/ecs/ecs.hpp>
#include <mee/engine-resources/engine-resources.hpp>

struct AABB {
    Vector2f min;
    Vector2f max;
};

struct CollisionObject {
    Entity entity;

    TransformComponent* transform;
    AABBColliderComponent* collider;
    RigidbodyComponent* rigidbody;
};


void aabbCollideSystem(ECS& ecs, EngineResources& res, float dt);
AABB getAABB(const TransformComponent& t, const AABBColliderComponent& c);

