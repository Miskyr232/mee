#include <algorithm>
#include <cstddef>
#include <mee/systems/physics/aabb-collision.hpp>
#include <mee/components/core/transform.hpp>
#include <mee/components/physics/aabb-collider.hpp>
#include <mee/components/physics/rigidbody.hpp>
#include <mee/utils/vector.hpp>
#include <vector>

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
    static std::vector<Body> bodies;
    bodies.clear();
    ecs.view<TransformComponent, AABBColliderComponent, RigidbodyComponent>(
        [&](Entity e, TransformComponent& transform, AABBColliderComponent& aabb, RigidbodyComponent& rb){
            bodies.push_back({
                e,
                &transform,
                &aabb,
                &rb,
                getAABB(transform, aabb)
            }
        );
    });
    std::sort(bodies.begin(), bodies.end(),
        [](const Body& a, const Body& b){ return a.box.min.x < b.box.min.x; });
    for(std::size_t i = 0; i < bodies.size(); i++){
        const float maxX = bodies[i].box.max.x;
        for(std::size_t j = i + 1; j < bodies.size() && bodies[j].box.min.x < maxX; ++j){
            Body& a = bodies[i];
            Body& b = bodies[j];

            if(b.box.min.y >= a.box.max.y ||
                b.box.max.y <= a.box.min.y)
                    continue;

            if(a.rigidbody->invMass == 0 && b.rigidbody->invMass == 0)
                continue;

            res.collisionSignal.emit(a.entity, b.entity);

            AABB aBox = getAABB(*bodies[i].transform, *bodies[i].collider);
            AABB bBox = getAABB(*bodies[j].transform, *bodies[j].collider);

            if(!intersects(aBox, bBox))
                continue;
            resolveAABB(*a.transform, *a.rigidbody, *a.collider, *b.transform, *b.rigidbody, *b.collider, a.box, b.box);
        }
    }
}
