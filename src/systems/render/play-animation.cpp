#include "mee/systems/render/play-animation.hpp"
#include "mee/components/render/animation.hpp"

void playAnimationSystem(ECS &ecs, EngineResources &res, float dt){
    ecs.view<AnimationComponent>(
        [&](Entity e, AnimationComponent& anim){
            if(anim.animation.nextFrameTimer.update(dt)){
                if(anim.animation.mode == PlaybackMode::Loop){
                    anim.animation.currentTextureIndex++;
                    if(anim.animation.currentTextureIndex >= anim.animation.textures.size()){
                        anim.animation.currentTextureIndex = 0;
                    }
                }if(anim.animation.mode == PlaybackMode::Once){
                    anim.animation.currentTextureIndex++;
                    if(anim.animation.currentTextureIndex >= anim.animation.textures.size()){
                        anim.animation.currentTextureIndex = 0;
                        anim.animation.mode = PlaybackMode::None;
                    }
                }
            }
        }
    );
}
