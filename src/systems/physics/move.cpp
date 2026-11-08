#include <mee/systems/physics/move.hpp>
#include <mee/components/core/transform.hpp>
#include <mee/components/physics/rigidbody.hpp>

void moveSystem(ECS &ecs, WorldResources &res, float dt){
    ecs.view<TransformComponent, RigidbodyComponent>(
        [&](Entity e) {
            auto& transform = ecs.getComponent<TransformComponent>(e);
            auto& rigidbody = ecs.getComponent<RigidbodyComponent>(e);

            rigidbody.acceleration =
                rigidbody.forceAccumulator * rigidbody.invMass;

            rigidbody.velocity += rigidbody.acceleration * dt;
            transform.pos += rigidbody.velocity.tof() * dt;

            rigidbody.velocity *=
                1.0f / (1.0f + rigidbody.drag * dt);

            rigidbody.forceAccumulator = Vector2d::zero();
            rigidbody.acceleration = Vector2d::zero();

            transform.rotation += rigidbody.angularVelocity * dt;
        }
    );
}
