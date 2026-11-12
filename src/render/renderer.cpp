#include <cstdint>
#include <mee/render/renderer.hpp>
#include <mee/render/render-command.hpp>
#include <mee/render/texture-manager.hpp>
#include <mee/utils/vector.hpp>
#include <mee/utils/profiler.hpp>
#include <mee/engine-resources/camera.hpp>
#include <cstdio>
#include <numeric>
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
    static const char* tileFs = R"(
        #version 330

        in vec2 fragTexCoord;
        in vec4 fragColor;

        out vec4 finalColor;

        uniform sampler2D texture0;
        uniform vec2 tileSize;

        void main() {
            vec2 uv = fragTexCoord / tileSize;

            finalColor = texture(texture0, uv) * fragColor;
        }
    )";
    circleShader = LoadShaderFromMemory(NULL, circleFs);
    rectShader = LoadShaderFromMemory(NULL, rectFs);
    ringShader = LoadShaderFromMemory(NULL, ringFs);
    tileShader = LoadShaderFromMemory(NULL, tileFs);
    tileSizeLoc = GetShaderLocation(tileShader, "tileSize");
    ringInRadiusLoc = GetShaderLocation(ringShader, "inRadius");

    circles.reserve(256);
    rects.reserve(256);
    rings.reserve(64);
    lines.reserve(256);
}

void Renderer::beginFrame(){
    order.clear();

    textCmds.clear();
    textureCmds.clear();
    lineCmds.clear();
    circleCmds.clear();
    ringCmds.clear();
    rectCmds.clear();

    nextSeq = 0;
    uniformZIndex = true;
}

void Renderer::flush(TextureManager& textureManager, WorldCamera& camera){
    Profiler::instance().measure(
        "sort render cmd",
        [&](){
            if(!uniformZIndex){
                std::sort(order.begin(), order.end(), [](const SortKey& a, const SortKey& b){
                    if(a.z != b.z) return a.z < b.z;
                    return a.seq < b.seq;
                });
            }
        }
    );
    circles.clear();
    rects.clear();
    rings.clear();
    lines.clear();
    for(const SortKey& k : order){
        switch (k.type) {
            case CmdType::Texture:{
                flushPending(camera);

                Texture2D texture = textureManager.getTexture(textureCmds[k.index].texture);

                Vector2f screenPos = camera.worldToScreen(textureCmds[k.index].pos);

                float zoom = camera.zoom;

                Rectangle source = {
                    0, 0,
                    (float)texture.width,
                    (float)texture.height
                };

                Rectangle dest = {
                    screenPos.x,
                    screenPos.y,
                    texture.width * zoom * textureCmds[k.index].scale.x,
                    texture.height * zoom * textureCmds[k.index].scale.y
                };

                Vector2 origin = {
                    texture.width * zoom * textureCmds[k.index].scale.x * textureCmds[k.index].origin.x,
                    texture.height * zoom * textureCmds[k.index].scale.y * textureCmds[k.index].origin.y
                };
                if(textureCmds[k.index].tileSize.x != 1.0f ||
                   textureCmds[k.index].tileSize.y != 1.0f){

                    if(!hasWrapTexture || lastWrapTextureId != texture.id){
                        SetTextureWrap(texture, TEXTURE_WRAP_REPEAT);
                        lastWrapTextureId = texture.id;
                        hasWrapTexture = true;
                    }

                    BeginShaderMode(tileShader);

                    SetShaderValue(
                        tileShader,
                        tileSizeLoc,
                        &textureCmds[k.index].tileSize,
                        SHADER_UNIFORM_VEC2
                    );

                    DrawTexturePro(
                        texture,
                        source,
                        dest,
                        origin,
                        textureCmds[k.index].rotation - camera.rotation,
                        textureCmds[k.index].tint.toRaylib()
                    );

                    EndShaderMode();

                }else{
                    DrawTexturePro(
                        texture,
                        source,
                        dest,
                        origin,
                        textureCmds[k.index].rotation - camera.rotation,
                        textureCmds[k.index].tint.toRaylib()
                    );
                }
                break;
            }
            case CmdType::Text:{
                flushPending(camera);
                Vector2f screenPos = textCmds[k.index].position;
                if(!k.isUI)
                    screenPos = camera.worldToScreen(textCmds[k.index].position);
                DrawText(
                    textCmds[k.index].text.c_str(),
                    screenPos.x,
                    screenPos.y,
                    textCmds[k.index].fontSize * camera.zoom,
                    textCmds[k.index].color.toRaylib()
                );
                break;
            }
            case CmdType::Rect:{
                if(!circles.empty() || !rings.empty() || !lines.empty())
                    flushPending(camera);
                rects.push_back(rectCmds[k.index]);
                break;
            }
            case CmdType::Circle:{
                if(!rects.empty() || !rings.empty() || !lines.empty())
                    flushPending(camera);
                circles.push_back(circleCmds[k.index]);
                break;
            }
            case CmdType::Ring:{
                if(!rects.empty() || !circles.empty() || !lines.empty())
                    flushPending(camera);
                rings.push_back(ringCmds[k.index]);
                break;
            }
            case CmdType::Line:{
                if(!rects.empty() || !circles.empty() || !rings.empty())
                    flushPending(camera);
                lines.push_back(lineCmds[k.index]);
                break;
            }
        }
    }
    flushCircles(circles, camera);
    flushRects(rects, camera);
    flushRings(rings, camera);
    flushLines(lines, camera);
}

void Renderer::renderTexture(TextureCmd cmd, float zIndex, bool isUI){
    if(order.empty()){
        firstZIndex = zIndex;
    }else if(zIndex != firstZIndex){
        uniformZIndex = false;
    }
    textureCmds.push_back(cmd);
    order.push_back({zIndex, (uint32_t)nextSeq++, CmdType::Texture, (uint32_t)textureCmds.size() - 1, isUI});
}
void Renderer::renderText(TextCmd cmd, float zIndex, bool isUI){
    if(order.empty()){
        firstZIndex = zIndex;
    }else if(zIndex != firstZIndex){
        uniformZIndex = false;
    }
    textCmds.push_back(cmd);
    order.push_back({zIndex, (uint32_t)nextSeq++, CmdType::Text, (uint32_t)textCmds.size() - 1, isUI});
}
void Renderer::renderCircle(CircleCmd cmd, float zIndex, bool isUI){
    if(order.empty()){
        firstZIndex = zIndex;
    }else if(zIndex != firstZIndex){
        uniformZIndex = false;
    }
    circleCmds.push_back(cmd);
    order.push_back({zIndex, (uint32_t)nextSeq++, CmdType::Circle, (uint32_t)circleCmds.size() - 1, isUI});
}
void Renderer::renderRing(RingCmd cmd, float zIndex, bool isUI){
    if(order.empty()){
        firstZIndex = zIndex;
    }else if(zIndex != firstZIndex){
        uniformZIndex = false;
    }
    ringCmds.push_back(cmd);
    order.push_back({zIndex, (uint32_t)nextSeq++, CmdType::Ring, (uint32_t)ringCmds.size() - 1, isUI});
}
void Renderer::renderRect(RectCmd cmd, float zIndex, bool isUI){
    if(order.empty()){
        firstZIndex = zIndex;
    }else if(zIndex != firstZIndex){
        uniformZIndex = false;
    }
    rectCmds.push_back(cmd);
    order.push_back({zIndex, (uint32_t)nextSeq++, CmdType::Rect, (uint32_t)rectCmds.size() - 1, isUI});
}
void Renderer::renderLine(LineCmd cmd, float zIndex, bool isUI){
    if(order.empty()){
        firstZIndex = zIndex;
    }else if(zIndex != firstZIndex){
        uniformZIndex = false;
    }
    lineCmds.push_back(cmd);
    order.push_back({zIndex, (uint32_t)nextSeq++, CmdType::Line, (uint32_t)lineCmds.size() - 1, isUI});
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
        Vector2f p = camera.worldToScreen(r.position);
        float w = r.size.x * camera.zoom;
        float h = r.size.y * camera.zoom;

        if(p.x > sw || p.x + w < 0 || p.y > sh || p.y + h < 0) continue;

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

    const float sw = (float)GetScreenWidth();
    const float sh = (float)GetScreenHeight();

    rlDrawRenderBatchActive();
    rlSetTexture(0);
    rlDisableBackfaceCulling();

    rlBegin(RL_QUADS);

    for(const auto& line : lines){
        if(line.thickness <= 0.0f) continue;

        Vector2f start = camera.worldToScreen(line.start);
        Vector2f end = camera.worldToScreen(line.end);

        if((start.x > sw && end.x > sw || start.y > sh && end.y > sh ) || (start.x < 0.0f && end.x < 0.0f || start.y < 0.0f && end.y < 0.0f)){
            continue;
        }

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
