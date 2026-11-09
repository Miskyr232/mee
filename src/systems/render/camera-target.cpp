#include <mee/systems/render/camera-target.hpp>
#include <mee/components/core/transform.hpp>
#include <mee/components/tags/camera-target.hpp>

void cameraTargetSystem(ECS& ecs, EngineResources& res, float dt){
    ecs.view<TransformComponent, CameraTargetTag>([&](Entity e, TransformComponent& tr, CameraTargetTag& t){
        Vector2f delta = (tr.pos - res.camera.pos);
        res.camera.pos += delta * 1.0f;
    });
}
