#include <mee/systems/physics/aabb-collision.hpp>
#include <mee/components/core/transform.hpp>
#include <mee/components/physics/aabb-collider.hpp>
#include <mee/components/physics/rigidbody.hpp>
#include <mee/utils/vector.hpp>
#include <vector>

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

AABB getAABB(
    const TransformComponent& t,
    const AABBColliderComponent& c
) {
    Vector2f center = t.pos + c.offset;
    Vector2f half = c.halfSize * t.scale;

    return {
        center - half,
        center + half
    };
}

bool intersects(const AABB& a, const AABB& b) {
    return a.min.x < b.max.x &&
           a.max.x > b.min.x &&
           a.min.y < b.max.y &&
           a.max.y > b.min.y;
}
void resolveAABB(
    TransformComponent& ta,
    RigidbodyComponent& ra,
    const AABBColliderComponent& ca,
    TransformComponent& tb,
    RigidbodyComponent& rb,
    const AABBColliderComponent& cb,
    const AABB& a,
    const AABB& b
) {
    double overlapX = std::min(a.max.x, b.max.x)
                    - std::max(a.min.x, b.min.x);

    double overlapY = std::min(a.max.y, b.max.y)
                    - std::max(a.min.y, b.min.y);

    Vector2f centerA = (a.min + a.max) / 2.0;
    Vector2f centerB = (b.min + b.max) / 2.0;

    Vector2f normal = Vector2f::zero();
    double overlap;

    if (overlapX < overlapY) {
        normal.x = centerA.x < centerB.x ? -1.0 : 1.0;
        overlap = overlapX;
    } else {
        normal.y = centerA.y < centerB.y ? -1.0 : 1.0;
        overlap = overlapY;
    }

    double invMassA = ra.invMass;
    double invMassB = rb.invMass;
    double totalInvMass = invMassA + invMassB;

    if (totalInvMass <= 0.0)
        return;

    Vector2f correction = normal * overlap;

    ta.pos.x += (correction * (invMassA / totalInvMass)).x;
    ta.pos.y += (correction * (invMassA / totalInvMass)).y;
    tb.pos.x -= (correction * (invMassB / totalInvMass)).x;
    tb.pos.y -= (correction * (invMassB / totalInvMass)).y;
    double restitution = std::min(
        ra.restitution,
        rb.restitution
    );

    Vector2d relativeVelocity = ra.velocity - rb.velocity;
    double velocityAlongNormal = Vector2d::dot(relativeVelocity, normal.tod());

    if (velocityAlongNormal >= 0.0)
        return;

    if (totalInvMass <= 0.0)
        return;

    double impulseMagnitude =
        -(1.0 + restitution) * velocityAlongNormal
        / totalInvMass;

    Vector2d impulse = normal.tod() * impulseMagnitude;

    ra.velocity += impulse * invMassA;
    rb.velocity -= impulse * invMassB;
}
void aabbCollideSystem(ECS &ecs, EngineResources &res, float dt){
    std::vector<CollisionObject> objects;
    ecs.view<TransformComponent, AABBColliderComponent, RigidbodyComponent>(
        [&](Entity e){
            objects.push_back({
                e,
                &ecs.getComponent<TransformComponent>(e),
                &ecs.getComponent<AABBColliderComponent>(e),
                &ecs.getComponent<RigidbodyComponent>(e)
            }
        );
    });
    for(int i = 0; i < objects.size(); i++){
        for (int j = i + 1; j < objects.size(); j++) {
            if(i == j)
                continue;
            CollisionObject a = objects[i];
            CollisionObject b = objects[j];
            AABB aabbA = getAABB(*a.transform, *a.collider);
            AABB aabbB = getAABB(*b.transform, *b.collider);

            if (!intersects(aabbA, aabbB))
                continue;
            res.collisionSignal.emit(a.entity, b.entity);
            resolveAABB(*a.transform, *a.rigidbody, *a.collider, *b.transform, *b.rigidbody, *b.collider, aabbA, aabbB);
        }
    }
}
