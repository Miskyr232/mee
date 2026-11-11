class Timer{
    float time = 0.0f;
    float duration = 1.0f;
public:
    Timer(float duration);
    Timer();
    bool update(float dt);
    void setDuration(float duration);
};
