#pragma once

#include <raylib.h>
#include <vector>
#include "render-command.hpp"
#include <mee/render/texture-manager.hpp>
#include <mee/engine-resources/camera.hpp>

class Renderer{
    std::vector<RenderCmd> commands;
    std::vector<CircleCmd> circles;
    std::vector<RectCmd> rects;
    std::vector<RingCmd> rings;
    std::vector<LineCmd> lines;
    Shader circleShader;
    Shader rectShader;
    Shader ringShader;
    int ringInRadiusLoc;
public:
    void init();
    void beginFrame();
    void render(RenderCmd smd);
    void renderTexture(TextureCmd cmd, float zIndex = 0, bool isUI = false);
    void renderCircle(CircleCmd cmd, float zIndex = 0, bool isUI = false);
    void renderText(TextCmd cmd, float zIndex = 0, bool isUI = false);
    void renderRing(RingCmd cmd, float zIndex = 0, bool isUI = false);
    void renderRect(RectCmd, float zIndex = 0, bool isUI = false);
    void renderLine(LineCmd, float zIndex = 0, bool isUI = false);
    void flush(TextureManager& textureManager, WorldCamera& camera);
    void flushCircles(const std::vector<CircleCmd>& circles, WorldCamera& camera);
    void flushRects(const std::vector<RectCmd>& rects, WorldCamera& camera);
    void flushRings(const std::vector<RingCmd>& rings, WorldCamera& camera);
    void flushLines(const std::vector<LineCmd>& lines, WorldCamera& camera);
    void flushPending(WorldCamera& camera);
};
