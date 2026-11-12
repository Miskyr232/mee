#include <chrono>
#include <cmath>
#include <cstdio>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <iostream>
#include "mee/utils/text.hpp"

struct ProfiledFunc{
    float last = 0.0f;
    float average = 0.0f;
    float acum = 0.0f;
    int amount = 0;
};

class Profiler{
    std::unordered_map<std::string, ProfiledFunc> profiledFuncs;
    float measureInterval = 1.0f; //sec
    float time = 0.0f;

    void updateAverageTime(){
        for(auto& pair : profiledFuncs){
            auto& p = pair.second;
            p.average = p.acum / (float)p.amount;
        }
        printInfo();
        for(auto& pair : profiledFuncs){
            auto& p = pair.second;
            p.acum = 0.0f;
            p.amount = 0;
        }
    }
public:
    static Profiler& instance(){
        static Profiler profiler;
        return profiler;
    };
    void record(const std::string& name, std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end){
        ProfiledFunc& p = profiledFuncs[name];

        float processTime =
            std::chrono::duration<float, std::milli>(end - start).count();

        p.last = processTime;
        p.acum += processTime;
        p.amount++;
    }
    template<typename Func>
    auto wrap(std::string name, Func&& func){
        return [this, name = std::move(name),
                func = std::forward<Func>(func)](auto&&... args) mutable {

            auto start = std::chrono::steady_clock::now();

            std::invoke(func, std::forward<decltype(args)>(args)...);

            auto end = std::chrono::steady_clock::now();

            record(name, start, end);
        };
    }
    template<typename Func>
    void measure(const std::string& name, Func&& func){
        auto start = std::chrono::steady_clock::now();

        std::invoke(std::forward<Func>(func));

        auto end = std::chrono::steady_clock::now();

        record(name, start, end);
    }
    void update(float dt){
        time += dt;
        if(time >= measureInterval){
            time = 0.0f;
            updateAverageTime();
        }
    };
    void printInfo(){
        printf("%s", Text::padRight("", 15).c_str());
        printf("%s", Text::padRight("last", 15).c_str());
        printf("%s", Text::padRight("average", 15).c_str());
        printf("%s\n", Text::padRight("amount", 15).c_str());
        for(auto pair : profiledFuncs){
            printf("%s\n", Text::repeat("-", 60).c_str());
            printf("%s", Text::padRight(pair.first, 15).c_str());
            printf("%s", (Text::padRight(Text::remove_trailing_zeros(pair.second.last, 3) + " ms", 15)).c_str());
            printf("%s", (Text::padRight(Text::remove_trailing_zeros(pair.second.average, 3) + " ms", 15)).c_str());
            printf("%s\n", Text::padRight(std::to_string(pair.second.amount), 15).c_str());
        }
        printf("\n");
    }
};
