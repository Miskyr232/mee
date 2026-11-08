#pragma once

#include <mee/utils/vector.hpp>
#include <raylib.h>

struct RigidbodyComponent{
    Vector2d velocity = Vector2d::zero();
    Vector2d acceleration = Vector2d::zero();
    Vector2d forceAccumulator = Vector2d::zero();

    float mass = 1.0f;
    float angularVelocity = 0.0f;
    float angularAccumulator = 0.0f;
    float inertiaTensor = 0.0f;
    float drag = 0.0f;

    float friction = 0.8f;

    float invMass = 1.0f;
    float invInertiaTensor = 1.0f;

    double restitution = 1.0;

    RigidbodyComponent() = default;
    RigidbodyComponent(float customMass, float customInertia = 1.0f) {
        mass = customMass;
        inertiaTensor = customInertia;

        invMass = (mass > 0.0f) ? (1.0f / mass) : 0.0f;
        invInertiaTensor = (inertiaTensor > 0.0f) ? (1.0f / inertiaTensor) : 0.0f;
    }
    void applyForce(Vector2d force){
        forceAccumulator += force;
    }
};
