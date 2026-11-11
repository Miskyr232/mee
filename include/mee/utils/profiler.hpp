#include <chrono>
#include <cstdio>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <iostream>

struct ProfiledFunc{
    float lastTime = 0.0f;
    float average = 0.0f;
    float acum = 0.0f;
    int measureAmount = 0;
};

class Profiler{
    std::unordered_map<std::string, ProfiledFunc> profiledFuncs;
    float measureInterval = 1.0f; //sec
    float time = 0.0f;

    void updateAverageTime(){
        for(auto& pair : profiledFuncs){
            auto& p = pair.second;
            p.average = p.acum / (float)p.measureAmount;
            p.acum = 0.0f;
            p.measureAmount = 0;
        }
    }
public:
    static Profiler& instance(){
        static Profiler profiler;
        return profiler;
    };
    void profile(const std::string_view name, auto&& func){
        auto start = std::chrono::steady_clock::now();
        std::invoke(std::forward<decltype(func)>(func));
        auto end = std::chrono::steady_clock::now();
        ProfiledFunc& p = profiledFuncs[std::string(name)];
        float processTime = std::chrono::duration<float>(end - start).count();
        p.lastTime = processTime;
        p.acum += processTime;
        p.measureAmount++;
    }
    void update(float dt){
        time += dt;
        if(time >= measureInterval){
            time = 0.0f;
            updateAverageTime();
        }
    };
    void printInfo(){
        for(auto pair : profiledFuncs){
            std::cout << pair.first << " " << pair.second.average << std::endl;
        }
        std::cout << std::endl;
    }
};
