#include "mee/utils/timer.hpp"

Timer::Timer(float duration_) : duration(duration_) {}

Timer::Timer() : duration(1.0f) {}

bool Timer::update(float dt){
    time += dt;
    if(time >= duration){
        time -= duration;
        return true;
    }
    return false;
}

void Timer::setDuration(float duration_){
    if(duration_ > 0.0f){
        duration = duration_;
    }
}
