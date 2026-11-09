#include "mee/components/core/transform.hpp"
#include "mee/components/render/sprite.hpp"
#include "mee/ecs/ecs.hpp"
#include "mee/core/world.hpp"
#include "mee/systems/render/render-sprite.hpp"
#include "mee/world-resources/world-resources.hpp"
#include "mee/systems/render/camera-target.hpp"
#include "mee/components/tags/camera-target.hpp"
#include <cstdio>
#include <filesystem>
#include <raylib.h>

struct PlayerTagComponent{

};

int main(){
    std::filesystem::current_path("examples/basic/assets");

    World w;

    Entity player = w.ecs.createEntity(
        TransformComponent{},
        SpriteComponent{
            .texture = w.res.textureManager.loadTexture("texture.png")
        },
        PlayerTagComponent{},
        CameraTargetTag{}
    );
    Entity wall = w.ecs.createEntity(
        TransformComponent{},
        SpriteComponent{
            .texture = w.res.textureManager.loadTexture("texture.png")
        }
    );

    w.pipeline.onDraw(renderSpriteSystem);

    w.run();
}
