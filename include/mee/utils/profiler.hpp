#include <algorithm>
#include <chrono>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "mee/utils/text.hpp"

struct ProfiledFunc{
    float last = 0.0f;
    float average = 0.0f;
    float acum = 0.0f;
    int amount = 0;
};

class Profiler{
    std::unordered_map<std::string, ProfiledFunc> profiledFuncs;
    std::vector<std::pair<std::string, ProfiledFunc>> toOut;

    float time = 0.0f;

    void updateAverageTime(){
        toOut.clear();
        for(auto& pair : profiledFuncs){
            auto& p = pair.second;
            p.average = p.amount > 0 ? p.acum / (float)p.amount : 0.0f;
            toOut.push_back({pair.first, pair.second});
        }
        for(auto& pair : profiledFuncs){
            auto& p = pair.second;
            p.acum = 0.0f;
            p.amount = 0;
        }
        std::sort(toOut.begin(), toOut.end(), [](std::pair<std::string, ProfiledFunc> first, std::pair<std::string, ProfiledFunc> second) {
            return first.second.average > second.second.average;
        });
    }
public:
    float measureInterval = 1.0f; //sec
    const std::vector<std::pair<std::string, ProfiledFunc>>& getData();
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
};
