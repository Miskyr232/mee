#include <algorithm>
#include <cmath>
#include <cstdint>
#include <mee/render/renderer.hpp>
#include <mee/render/render-command.hpp>
#include <mee/render/texture-manager.hpp>
#include <mee/utils/vector.hpp>
#include <mee/utils/profiler.hpp>
#include <mee/engine-resources/camera.hpp>
#include <raylib.h>
#include <rlgl.h>
#include <vector>

namespace {
    constexpr int RING_SEGMENTS = 32;
}

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
        void main(){ finalColor = fragColor; }
    )";

    static const char* tileFs = R"(
        #version 330
        in vec2 fragTexCoord;
        in vec4 fragColor;
        out vec4 finalColor;
        uniform sampler2D texture0;
        uniform vec2 tileSize;
        void main(){
            vec2 uv = fragTexCoord * tileSize;
            finalColor = texture(texture0, uv) * fragColor;
        }
    )";

    circleShader = LoadShaderFromMemory(NULL, circleFs);
    rectShader = LoadShaderFromMemory(NULL, rectFs);
    tileShader = LoadShaderFromMemory(NULL, tileFs);
    tileSizeLoc = GetShaderLocation(tileShader, "tileSize");

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
    Profiler::instance().measure("sort render cmd", [&](){
        if(!uniformZIndex){
            std::sort(order.begin(), order.end(),
                [](const SortKey& a, const SortKey& b){
                    if(a.z != b.z) return a.z < b.z;
                    return a.seq < b.seq;
                });
        }
    });

    circles.clear();
    rects.clear();
    rings.clear();
    lines.clear();

    struct PendingTex{
        Vector2f pos;
        float w;
        float h;
        float ox;
        float oy;
        float rotRad;
        Color col;
    };
    std::vector<PendingTex> texBatch;
    texBatch.reserve(256);
    uint32_t batchTexId = 0;

    auto flushTextureBatch = [&](){
        if(texBatch.empty())
            return;

        Profiler::instance().measure("render texture", [&](){
            rlDisableBackfaceCulling();
            rlSetTexture(batchTexId);
            rlBegin(RL_QUADS);

            for(const PendingTex& t : texBatch){
                rlCheckRenderBatchLimit(4);

                float c = cosf(t.rotRad);
                float s = sinf(t.rotRad);

                rlColor4ub(t.col.r, t.col.g, t.col.b, t.col.a);

                float qx[4] = {0.0f, 0.0f, t.w, t.w};
                float qy[4] = {0.0f, t.h, t.h, 0.0f};
                float uvx[4] = {0.0f, 0.0f, 1.0f, 1.0f};
                float uvy[4] = {0.0f, 1.0f, 1.0f, 0.0f};

                for(int i = 0; i < 4; i++){
                    float dx = qx[i] - t.ox;
                    float dy = qy[i] - t.oy;
                    rlTexCoord2f(uvx[i], uvy[i]);
                    rlVertex2f(t.pos.x + dx * c - dy * s, t.pos.y + dx * s + dy * c);
                }
            }

            rlEnd();
            rlEnableBackfaceCulling();
        });
        texBatch.clear();
    };

    auto flushAll = [&](){
        flushTextureBatch();
        flushPending(camera);
    };

    renderCallsAmount = order.size();

    for(const SortKey& k : order){
        switch(k.type){
            case CmdType::Texture:{
                const auto& cmd = textureCmds[k.index];
                Texture2D texture = textureManager.getTexture(cmd.texture);

                if(cmd.tileSize.x != 1.0f || cmd.tileSize.y != 1.0f){
                    flushAll();

                    if(!hasWrapTexture || lastWrapTextureId != texture.id){
                        SetTextureWrap(texture, TEXTURE_WRAP_REPEAT);
                        lastWrapTextureId = texture.id;
                        hasWrapTexture = true;
                    }

                    Profiler::instance().measure("render texture", [&](){
                        BeginShaderMode(tileShader);
                        SetShaderValue(tileShader, tileSizeLoc,
                                       &cmd.tileSize, SHADER_UNIFORM_VEC2);

                        Vector2f screenPos = camera.worldToScreen(cmd.pos);
                        float zoom = camera.zoom;

                        Rectangle source = {
                            0.0f, 0.0f,
                            (float)texture.width, (float)texture.height
                        };
                        Rectangle dest = {
                            screenPos.x, screenPos.y,
                            texture.width * zoom * cmd.scale.x * cmd.tileSize.x,
                            texture.height * zoom * cmd.scale.y * cmd.tileSize.y
                        };
                        Vector2 origin = {
                            dest.width * cmd.origin.x,
                            dest.height * cmd.origin.y
                        };

                        DrawTexturePro(texture, source, dest, origin,
                                       cmd.rotation - camera.rotation,
                                       cmd.tint.toRaylib());
                        EndShaderMode();
                    });
                    break;
                }

                if(texture.id != (int)batchTexId)
                    flushTextureBatch();

                if(!circles.empty() || !rings.empty() || !lines.empty() || !rects.empty())
                    flushPending(camera);

                if(texBatch.empty())
                    batchTexId = texture.id;

                Vector2f screenPos = camera.worldToScreen(cmd.pos);
                float zoom = camera.zoom;
                float w = texture.width * zoom * cmd.scale.x;
                float h = texture.height * zoom * cmd.scale.y;

                texBatch.push_back({
                    screenPos,
                    w,
                    h,
                    w * cmd.origin.x,
                    h * cmd.origin.y,
                    (cmd.rotation - camera.rotation) * DEG2RAD,
                    cmd.tint.toRaylib()
                });
                break;
            }

            case CmdType::Text:{
                flushAll();
                const auto& cmd = textCmds[k.index];
                Vector2f screenPos = cmd.position;
                float size = cmd.fontSize;

                if(!k.isUI){
                    screenPos = camera.worldToScreen(cmd.position);
                    size *= camera.zoom;
                }

                DrawText(cmd.text.c_str(), screenPos.x, screenPos.y,
                         size, cmd.color.toRaylib());
                break;
            }

            case CmdType::Rect:{
                if(!texBatch.empty() || !circles.empty() || !rings.empty() || !lines.empty())
                    flushAll();

                RectCmd cmd = rectCmds[k.index];
                if(k.isUI){
                    cmd.position = camera.screenToWorld(cmd.position);
                    cmd.size /= camera.zoom;
                }
                rects.push_back(cmd);
                break;
            }

            case CmdType::Circle:{
                if(!texBatch.empty() || !rects.empty() || !rings.empty() || !lines.empty())
                    flushAll();
                circles.push_back(circleCmds[k.index]);
                break;
            }

            case CmdType::Ring:{
                if(!texBatch.empty() || !rects.empty() || !circles.empty() || !lines.empty())
                    flushAll();
                rings.push_back(ringCmds[k.index]);
                break;
            }

            case CmdType::Line:{
                if(!texBatch.empty() || !rects.empty() || !circles.empty() || !rings.empty())
                    flushAll();
                lines.push_back(lineCmds[k.index]);
                break;
            }
        }
    }

    flushAll();
}

void Renderer::renderTexture(TextureCmd cmd, float zIndex, bool isUI){
    if(order.empty()) firstZIndex = zIndex;
    else if(zIndex != firstZIndex) uniformZIndex = false;
    textureCmds.push_back(cmd);
    order.push_back({zIndex, (uint32_t)nextSeq++, CmdType::Texture,
                     (uint32_t)textureCmds.size() - 1, isUI});
}

void Renderer::renderText(TextCmd cmd, float zIndex, bool isUI){
    if(order.empty()) firstZIndex = zIndex;
    else if(zIndex != firstZIndex) uniformZIndex = false;
    textCmds.push_back(cmd);
    order.push_back({zIndex, (uint32_t)nextSeq++, CmdType::Text,
                     (uint32_t)textCmds.size() - 1, isUI});
}

void Renderer::renderCircle(CircleCmd cmd, float zIndex, bool isUI){
    if(order.empty()) firstZIndex = zIndex;
    else if(zIndex != firstZIndex) uniformZIndex = false;
    circleCmds.push_back(cmd);
    order.push_back({zIndex, (uint32_t)nextSeq++, CmdType::Circle,
                     (uint32_t)circleCmds.size() - 1, isUI});
}

void Renderer::renderRing(RingCmd cmd, float zIndex, bool isUI){
    if(order.empty()) firstZIndex = zIndex;
    else if(zIndex != firstZIndex) uniformZIndex = false;
    ringCmds.push_back(cmd);
    order.push_back({zIndex, (uint32_t)nextSeq++, CmdType::Ring,
                     (uint32_t)ringCmds.size() - 1, isUI});
}

void Renderer::renderRect(RectCmd cmd, float zIndex, bool isUI){
    if(order.empty()) firstZIndex = zIndex;
    else if(zIndex != firstZIndex) uniformZIndex = false;
    rectCmds.push_back(cmd);
    order.push_back({zIndex, (uint32_t)nextSeq++, CmdType::Rect,
                     (uint32_t)rectCmds.size() - 1, isUI});
}

void Renderer::renderLine(LineCmd cmd, float zIndex, bool isUI){
    if(order.empty()) firstZIndex = zIndex;
    else if(zIndex != firstZIndex) uniformZIndex = false;
    lineCmds.push_back(cmd);
    order.push_back({zIndex, (uint32_t)nextSeq++, CmdType::Line,
                     (uint32_t)lineCmds.size() - 1, isUI});
}

void Renderer::flushCircles(const std::vector<CircleCmd>& commands,
                            WorldCamera& camera){
    if(commands.empty()) return;
    const float sw = (float)GetScreenWidth();
    const float sh = (float)GetScreenHeight();

    BeginShaderMode(circleShader);
    rlSetTexture(0);
    rlDisableBackfaceCulling();
    rlBegin(RL_QUADS);

    for(const auto& c : commands){
        Vector2f p = camera.worldToScreen(c.center);
        float r = c.radius * camera.zoom;
        if(p.x + r < 0 || p.x - r > sw ||
           p.y + r < 0 || p.y - r > sh) continue;

        rlCheckRenderBatchLimit(4);
        Color col = c.color.toRaylib();
        rlColor4ub(col.r, col.g, col.b, col.a);
        rlTexCoord2f(0, 0); rlVertex2f(p.x-r, p.y-r);
        rlTexCoord2f(0, 1); rlVertex2f(p.x-r, p.y+r);
        rlTexCoord2f(1, 1); rlVertex2f(p.x+r, p.y+r);
        rlTexCoord2f(1, 0); rlVertex2f(p.x+r, p.y-r);
    }

    rlEnd();
    EndShaderMode();
    rlEnableBackfaceCulling();
}

void Renderer::flushRects(const std::vector<RectCmd>& commands,
                          WorldCamera& camera){
    if(commands.empty()) return;
    const float sw = (float)GetScreenWidth();
    const float sh = (float)GetScreenHeight();

    BeginShaderMode(rectShader);
    rlSetTexture(0);
    rlDisableBackfaceCulling();
    rlBegin(RL_QUADS);

    for(const auto& r : commands){
        Vector2f p = camera.worldToScreen(r.position);
        float w = r.size.x * camera.zoom;
        float h = r.size.y * camera.zoom;
        if(p.x > sw || p.x+w < 0 || p.y > sh || p.y+h < 0) continue;

        rlCheckRenderBatchLimit(4);
        Color col = r.color.toRaylib();
        rlColor4ub(col.r, col.g, col.b, col.a);
        rlTexCoord2f(0, 0); rlVertex2f(p.x, p.y);
        rlTexCoord2f(0, 1); rlVertex2f(p.x, p.y+h);
        rlTexCoord2f(1, 1); rlVertex2f(p.x+w, p.y+h);
        rlTexCoord2f(1, 0); rlVertex2f(p.x+w, p.y);
    }

    rlEnd();
    EndShaderMode();
    rlEnableBackfaceCulling();
}

void Renderer::flushRings(const std::vector<RingCmd>& commands,
                          WorldCamera& camera){
    if(commands.empty()) return;
    const float sw = (float)GetScreenWidth();
    const float sh = (float)GetScreenHeight();

    // One batch, no per-ring uniform updates or forced batch flushes.
    BeginShaderMode(rectShader);
    rlSetTexture(0);
    rlDisableBackfaceCulling();
    rlBegin(RL_QUADS);

    for(const auto& r : commands){
        Vector2f p = camera.worldToScreen(r.center);
        float outer = r.outRadius * camera.zoom;
        float inner = r.inRadius * camera.zoom;

        if(outer <= 0.0f) continue;
        if(p.x+outer < 0 || p.x-outer > sw ||
           p.y+outer < 0 || p.y-outer > sh) continue;

        inner = std::clamp(inner, 0.0f, outer);
        Color col = r.color.toRaylib();
        if(col.a == 0) continue;
        rlColor4ub(col.r, col.g, col.b, col.a);

        for(int i = 0; i < RING_SEGMENTS; ++i){
            float a0 = 2.0f * PI * i / RING_SEGMENTS;
            float a1 = 2.0f * PI * (i+1) / RING_SEGMENTS;
            float c0 = std::cos(a0), s0 = std::sin(a0);
            float c1 = std::cos(a1), s1 = std::sin(a1);

            rlCheckRenderBatchLimit(4);
            rlTexCoord2f(0, 0); rlVertex2f(p.x+c0*outer, p.y+s0*outer);
            rlTexCoord2f(0, 1); rlVertex2f(p.x+c1*outer, p.y+s1*outer);
            rlTexCoord2f(1, 1); rlVertex2f(p.x+c1*inner, p.y+s1*inner);
            rlTexCoord2f(1, 0); rlVertex2f(p.x+c0*inner, p.y+s0*inner);
        }
    }

    rlEnd();
    EndShaderMode();
    rlEnableBackfaceCulling();
}

void Renderer::flushLines(const std::vector<LineCmd>& commands,
                          WorldCamera& camera){
    if(commands.empty()) return;
    const float sw = (float)GetScreenWidth();
    const float sh = (float)GetScreenHeight();

    rlSetTexture(0);
    rlDisableBackfaceCulling();
    rlBegin(RL_QUADS);

    for(const auto& line : commands){
        if(line.thickness <= 0.0f) continue;
        Vector2f start = camera.worldToScreen(line.start);
        Vector2f end = camera.worldToScreen(line.end);

        float dx = end.x-start.x;
        float dy = end.y-start.y;
        float len = std::sqrt(dx*dx + dy*dy);
        if(len <= 0.0001f) continue;

        float half = line.thickness * camera.zoom * 0.5f;
        float nx = -dy/len * half;
        float ny = dx/len * half;

        float minX = std::min(start.x, end.x)-half;
        float maxX = std::max(start.x, end.x)+half;
        float minY = std::min(start.y, end.y)-half;
        float maxY = std::max(start.y, end.y)+half;
        if(maxX < 0 || minX > sw || maxY < 0 || minY > sh) continue;

        Color col = line.color.toRaylib();
        if(col.a == 0) continue;
        rlCheckRenderBatchLimit(4);
        rlColor4ub(col.r, col.g, col.b, col.a);
        rlTexCoord2f(0, 0); rlVertex2f(start.x+nx, start.y+ny);
        rlTexCoord2f(0, 1); rlVertex2f(end.x+nx, end.y+ny);
        rlTexCoord2f(1, 1); rlVertex2f(end.x-nx, end.y-ny);
        rlTexCoord2f(1, 0); rlVertex2f(start.x-nx, start.y-ny);
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

int Renderer::getRenderCallsAmount() const{
    return renderCallsAmount;
}
