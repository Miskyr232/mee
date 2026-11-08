#pragma once

#include <chrono>
#include <iostream>
#include <functional>
#include <utility>

template<typename F, typename... Args>
auto measureTime(const char* name, F&& func, Args&&... args) {
    using namespace std::chrono;

    const auto start = steady_clock::now();

    if constexpr (std::is_void_v<
        std::invoke_result_t<F, Args...>
    >) {
        std::invoke(
            std::forward<F>(func),
            std::forward<Args>(args)...
        );

        const auto end = steady_clock::now();
        const auto elapsed = duration<double, std::milli>(end - start);

        std::cout << name << ": " << elapsed.count() << " ms\n";
    } else {
        auto result = std::invoke(
            std::forward<F>(func),
            std::forward<Args>(args)...
        );

        const auto end = steady_clock::now();
        const auto elapsed = duration<double, std::milli>(end - start);

        std::cout << name << ": " << elapsed.count() << " ms\n";

        return result;
    }
}
template<typename F>
auto timedSystem(const char* name, F system) {
    return [name, system = std::move(system)](auto&&... args) mutable {
        const auto start = std::chrono::steady_clock::now();

        system(std::forward<decltype(args)>(args)...);

        const auto end = std::chrono::steady_clock::now();
        const auto elapsed =
            std::chrono::duration<double, std::milli>(end - start);

        std::cout << name << ": " << elapsed.count() << " ms\n";
    };
}
