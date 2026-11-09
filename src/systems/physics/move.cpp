#include <mee/systems/physics/move.hpp>
#include <mee/components/core/transform.hpp>
#include <mee/components/physics/rigidbody.hpp>

void moveSystem(ECS &ecs, EngineResources &res, float dt){
    ecs.view<TransformComponent, RigidbodyComponent>(
        [&](Entity e, TransformComponent& t, RigidbodyComponent& r) {
            r.acceleration =
                r.forceAccumulator * r.invMass;

            r.velocity += r.acceleration * dt;
            t.pos += r.velocity.tof() * dt;

            r.velocity *=
                1.0f / (1.0f + r.drag * dt);

            r.forceAccumulator = Vector2d::zero();
            r.acceleration = Vector2d::zero();

            t.rotation += r.angularVelocity * dt;
        }
    );
}
