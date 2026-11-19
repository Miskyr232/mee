#pragma once

struct Timer{
    float time = 0.0f;
    float duration = 1.0f;
    bool update(float dt);
    void setDuration(float duration);
};
