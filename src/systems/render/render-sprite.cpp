#include <cstdio>
#include <mee/components/render/sprite.hpp>
#include <mee/components/core/transform.hpp>
#include <mee/world-resources/world-resources.hpp>
#include <algorithm>
#include <raylib.h>
#include <mee/systems/render/render-sprite.hpp>

void renderSpriteSystem(ECS& ecs, WorldResources& res) {
    ecs.view<TransformComponent, SpriteComponent>([&](Entity e, TransformComponent& t, SpriteComponent& s) {
        Texture2D texture = res.textureManager.getTexture(s.texture);
        res.renderer.renderTexture(
            {
                s.texture,
                t.pos,
                t.scale,
                Vector2f(0.5f, 0.5f),
                t.rotation,
                Color4f::White()
            },
            s.zIndex,
            false
        );
    });
}

