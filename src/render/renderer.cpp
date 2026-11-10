#include <mee/render/renderer.hpp>
#include <mee/render/render-command.hpp>
#include <mee/render/texture-manager.hpp>
#include <mee/utils/vector.hpp>
#include <mee/engine-resources/camera.hpp>
#include <algorithm>
#include <cstdio>
#include <raylib.h>
#include <rlgl.h>
#include <variant>
#include <vector>

void Renderer::init(){
    static const char* circleFs = R"(
        #version 330
        in vec2 fragTexCoord;
        in vec4 fragColor;
        out vec4 finalColor;
        void main(){
            float d = length(fragTexCoord - vec2(0.5));
            float alpha = step(d, 0.5);
            finalColor = vec4(fragColor.rgb, fragColor.a * alpha);
        }
    )";
    static const char* rectFs = R"(
        #version 330
        in vec2 fragTexCoord;
        in vec4 fragColor;
        out vec4 finalColor;
        void main(){
            finalColor = fragColor;
        }
    )";
    static const char* ringFs = R"(
        #version 330
        in vec2 fragTexCoord;
        in vec4 fragColor;
        out vec4 finalColor;
        uniform float inRadius;
        void main(){
            float d = length(fragTexCoord - vec2(0.5));

            float alpha = step(inRadius, d) * step(d, 0.5);

            if(alpha <= 0.0) discard;

            finalColor = fragColor;
        }
    )";
    circleShader = LoadShaderFromMemory(NULL, circleFs);
    rectShader = LoadShaderFromMemory(NULL, rectFs);
    ringShader = LoadShaderFromMemory(NULL, ringFs);
    ringInRadiusLoc = GetShaderLocation(ringShader, "inRadius");
}

void Renderer::beginFrame(){
    commands.clear();
}

void Renderer::render(RenderCmd cmd){
    commands.push_back(std::move(cmd));
}

void Renderer::flush(TextureManager& textureManager, WorldCamera& camera){
    std::stable_sort(commands.begin(), commands.end(),
                     [](const RenderCmd& a, const RenderCmd& b){
                        return a.zIndex < b.zIndex;
                     });
    circles.clear();
    rects.clear();
    rings.clear();
    lines.clear();
    for(const auto& command : commands){
        if(auto* circleCmd = std::get_if<CircleCmd>(&command.variant)){
            if(!rects.empty()){
                flushPending(camera);
            }if(!rings.empty()){
                flushPending(camera);
            }if(!lines.empty()){
                flushPending(camera);
            }
            circles.push_back(*circleCmd);
        }else if(auto* rectCmd = std::get_if<RectCmd>(&command.variant)){
            if(!circles.empty()){
                flushPending(camera);
            }if(!rings.empty()){
                flushPending(camera);
            }if(!lines.empty()){
                flushPending(camera);
            }
            rects.push_back(*rectCmd);
        }else if(auto* ringCmd = std::get_if<RingCmd>(&command.variant)){
            if(!circles.empty()){
                flushPending(camera);
            }if(!rects.empty()){
                flushPending(camera);
            }if(!lines.empty()){
                flushPending(camera);
            }
            rings.push_back(*ringCmd);
        }else if(auto* lineCmd = std::get_if<LineCmd>(&command.variant)){
            if(!circles.empty()){
                flushPending(camera);
            }if(!rects.empty()){
                flushPending(camera);
            }if(!rings.empty()){
                flushPending(camera);
            }
            lines.push_back(*lineCmd);
        }else if(auto* textCmd = std::get_if<TextCmd>(&command.variant)){
            flushPending(camera);
            Vector2f screenPos = textCmd->position;
            if(!command.isUI)
                screenPos = camera.worldToScreen(textCmd->position);
            DrawText(
                textCmd->text.c_str(),
                screenPos.x,
                screenPos.y,
                textCmd->fontSize * camera.zoom,
                textCmd->color.toRaylib()
            );
        }else if(auto* textureCmd = std::get_if<TextureCmd>(&command.variant)){
            flushPending(camera);
            Texture2D texture = textureManager.getTexture(textureCmd->texture);
            Vector2f screenPos = camera.worldToScreen(textureCmd->pos);

            Rectangle source = {
                0.0f, 0.0f,
                (float)texture.width,
                (float)texture.height
            };

            float zoom = camera.zoom;
            Rectangle dest = {
                screenPos.x, screenPos.y,
                (float)texture.width * zoom * textureCmd->scale.x,
                (float)texture.height * zoom * textureCmd->scale.y
            };
            Vector2 origin = {
                texture.width * zoom * textureCmd->scale.x * textureCmd->origin.x,
                texture.height * zoom * textureCmd->scale.y * textureCmd->origin.y
            };
            DrawTexturePro(
                texture,
                source,
                dest,
                origin,
                textureCmd->rotation - camera.rotation,
                textureCmd->tint.toRaylib()
            );
        }
    }
    flushCircles(circles, camera);
    flushRects(rects, camera);
    flushRings(rings, camera);
    flushLines(lines, camera);
}

void Renderer::renderTexture(TextureCmd cmd, float zIndex, bool isUI){
    render({
        zIndex,
        isUI,
        cmd
    });
}
void Renderer::renderText(TextCmd cmd, float zIndex, bool isUI){
    render({
        zIndex,
        isUI,
        cmd
    });
}
void Renderer::renderCircle(CircleCmd cmd, float zIndex, bool isUI){
    render({
        zIndex,
        isUI,
        cmd
    });
}
void Renderer::renderRing(RingCmd cmd, float zIndex, bool isUI){
    render({
        zIndex,
        isUI,
        cmd
    });
}
void Renderer::renderRect(RectCmd cmd, float zIndex, bool isUI){
    render({
        zIndex,
        isUI,
        cmd
    });
}
void Renderer::renderLine(LineCmd cmd, float zIndex, bool isUI){
    render({
        zIndex,
        isUI,
        cmd
    });
}
void Renderer::flushCircles(const std::vector<CircleCmd>& circles, WorldCamera& camera){
    if(circles.empty()) return;

    const float sw = (float)GetScreenWidth();
    const float sh = (float)GetScreenHeight();

    BeginShaderMode(circleShader);
    rlSetTexture(0);
    rlDisableBackfaceCulling();

    rlBegin(RL_QUADS);

    for(const auto& c : circles){
        Vector2f p = camera.worldToScreen(c.center);
        float r = c.radius * camera.zoom;

        if(p.x + r < 0 || p.x - r > sw || p.y + r < 0 || p.y - r > sh) continue;

        rlCheckRenderBatchLimit(4);
        Color col = c.color.toRaylib();
        rlColor4ub(col.r, col.g, col.b, col.a);
        rlTexCoord2f(0, 0); rlVertex2f(p.x - r, p.y - r);
        rlTexCoord2f(0, 1); rlVertex2f(p.x - r, p.y + r);
        rlTexCoord2f(1, 1); rlVertex2f(p.x + r, p.y + r);
        rlTexCoord2f(1, 0); rlVertex2f(p.x + r, p.y - r);
    }

    rlEnd();
    EndShaderMode();
    rlEnableBackfaceCulling();
}

void Renderer::flushRects(const std::vector<RectCmd>& rects, WorldCamera& camera){
    if(rects.empty()) return;

    const float sw = (float)GetScreenWidth();
    const float sh = (float)GetScreenHeight();

    BeginShaderMode(rectShader);
    rlSetTexture(0);
    rlDisableBackfaceCulling();

    rlBegin(RL_QUADS);

    for(const auto& r : rects){
        Vector2f p = camera.worldToScreen(r.positon);
        float w = r.size.x * camera.zoom;
        float h = r.size.y * camera.zoom;

        if(p.x + w < 0 || p.x - w > sw || p.y + h < 0 || p.y - h > sh) continue;

        rlCheckRenderBatchLimit(4);
        Color col = r.color.toRaylib();
        rlColor4ub(col.r, col.g, col.b, col.a);
        rlTexCoord2f(0, 0); rlVertex2f(p.x, p.y);
        rlTexCoord2f(0, 1); rlVertex2f(p.x, p.y + h);
        rlTexCoord2f(1, 1); rlVertex2f(p.x + w, p.y + h);
        rlTexCoord2f(1, 0); rlVertex2f(p.x + w, p.y);
    }

    rlEnd();
    EndShaderMode();
    rlEnableBackfaceCulling();
}

void Renderer::flushRings(const std::vector<RingCmd>& rings, WorldCamera& camera){
    if(rings.empty()) return;

    const float sw = (float)GetScreenWidth();
    const float sh = (float)GetScreenHeight();

    BeginShaderMode(ringShader);
    rlSetTexture(0);
    rlDisableBackfaceCulling();

    rlBegin(RL_QUADS);

    for(const auto& r : rings){
        Vector2f p = camera.worldToScreen(r.center);
        float inR = r.inRadius * camera.zoom;
        float outR = r.outRadius * camera.zoom;

        if(p.x + outR < 0 || p.x - outR > sw || p.y + outR < 0 || p.y - outR > sh) continue;

        float normalizedInRadius = inR / outR * 0.5f;

        rlDrawRenderBatchActive();
        SetShaderValue(ringShader, ringInRadiusLoc, &normalizedInRadius, SHADER_UNIFORM_FLOAT);

        rlCheckRenderBatchLimit(4);
        Color col = r.color.toRaylib();
        rlColor4ub(col.r, col.g, col.b, col.a);
        rlTexCoord2f(0, 0); rlVertex2f(p.x - outR, p.y - outR);
        rlTexCoord2f(0, 1); rlVertex2f(p.x - outR, p.y + outR);
        rlTexCoord2f(1, 1); rlVertex2f(p.x + outR, p.y + outR);
        rlTexCoord2f(1, 0); rlVertex2f(p.x + outR, p.y - outR);
    }

    rlEnd();
    EndShaderMode();
    rlEnableBackfaceCulling();
}

void Renderer::flushLines(const std::vector<LineCmd>& lines, WorldCamera& camera){
    if(lines.empty()) return;

    rlDrawRenderBatchActive();
    rlSetTexture(0);
    rlDisableBackfaceCulling();

    rlBegin(RL_QUADS);

    for(const auto& line : lines){
        if(line.thickness <= 0.0f) continue;

        Vector2f start = camera.worldToScreen(line.start);
        Vector2f end = camera.worldToScreen(line.end);

        float dx = end.x - start.x;
        float dy = end.y - start.y;

        float len = std::sqrt(dx * dx + dy * dy);
        if(len <= 0.0001f) continue;

        float halfThickness = line.thickness * camera.zoom * 0.5f;

        float nx = -dy / len * halfThickness;
        float ny = dx / len * halfThickness;

        Color col = line.color.toRaylib();
        if(col.a == 0) continue;

        rlCheckRenderBatchLimit(4);

        rlColor4ub(col.r, col.g, col.b, col.a);

        rlTexCoord2f(0, 0); rlVertex2f(start.x + nx, start.y + ny);
        rlTexCoord2f(0, 1); rlVertex2f(end.x + nx, end.y + ny);
        rlTexCoord2f(1, 1); rlVertex2f(end.x - nx, end.y - ny);
        rlTexCoord2f(1, 0); rlVertex2f(start.x - nx, start.y - ny);
    }

    rlEnd();
    rlEnableBackfaceCulling();
}
void Renderer::flushPending(WorldCamera& camera){
    if(!circles.empty()){
        flushCircles(circles, camera);
        circles.clear();
    }
    if(!rects.empty()){
        flushRects(rects, camera);
        rects.clear();
    }
    if(!rings.empty()){
        flushRings(rings, camera);
        rings.clear();
    }
    if(!lines.empty()){
        flushLines(lines, camera);
        lines.clear();
    }
}
