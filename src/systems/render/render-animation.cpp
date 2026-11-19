#include "mee/systems/render/render-animation.hpp"
#include "mee/ecs/ecs.hpp"
#include "mee/engine-resources/engine-resources.hpp"
#include "mee/components/render/animation.hpp"
#include "mee/components/core/transform.hpp"
#include "mee/utils/vector.hpp"

void renderAnimationSystem(ECS& ecs, EngineResources& res){
    ecs.view<TransformComponent, AnimationComponent>(
        [&](Entity e, TransformComponent& tr, AnimationComponent& anim){
            if(anim.animation.currentTextureIndex < 0 || anim.animation.currentTextureIndex >= anim.animation.textures.size())
                return;
            res.renderer.renderTexture(
                {
                    anim.animation.textures[anim.animation.currentTextureIndex],
                    tr.pos,
                    tr.scale,
                    Vector2f(0.5f, 0.5f),
                    tr.rotation
                },
                anim.zIndex,
                anim.isUI
            );
        }
    );
}
