#pragma once

#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stddef.h>

namespace detail {

template <typename T> struct Rect {
    T x{};
    T y{};
    T w{};
    T h{};

    constexpr Rect() = default;
    constexpr Rect(T x, T y, T w, T h) : x(x), y(y), w(w), h(h) {}

    constexpr Rect operator+(const Rect &other) const {
        return {x + other.x, y + other.y, w + other.w, h + other.h};
    }

    constexpr Rect operator-(const Rect &other) const {
        return {x - other.x, y - other.y, w - other.w, h - other.h};
    }

    constexpr Rect operator*(T scalar) const {
        return {x * scalar, y * scalar, w * scalar, h * scalar};
    }

    constexpr Rect operator/(T scalar) const {
        return {x / scalar, y / scalar, w / scalar, h / scalar};
    }

    constexpr Rect operator*(Rect<T> other) const {
        return {x * other.x, y * other.y, w + other.w, h + other.h};
    }

    constexpr Rect &operator+=(const Rect &other) {
        x += other.x;
        y += other.y;
        h += other.h;
        w += other.w;
        return *this;
    }

    constexpr Rect &operator-=(const Rect &other) {
        x -= other.x;
        y -= other.y;
        h -= other.h;
        w -= other.w;
        return *this;
    }

    constexpr Rect &operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        h *= scalar;
        w *= scalar;
        return *this;
    }

    constexpr Rect &operator/=(T scalar) {
        x /= scalar;
        y /= scalar;
        h /= scalar;
        w /= scalar;
        return *this;
    }

    constexpr bool operator==(const Rect &other) const {
        return x == other.x && y == other.y && h == other.h && w == other.w;
    }

    constexpr bool operator!=(const Rect &other) const {
        return !(*this == other);
    }

    constexpr Rect(const Rect &other)
      : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), w(static_cast<T>(other.w)), h(static_cast<T>(other.h)) {}

    explicit constexpr operator ::Rectangle() const {
        return {static_cast<float>(x), static_cast<float>(y), w(static_cast<T>(w), h(static_cast<T>(h)))};
    }
    constexpr Rectangle toRaylib() const {
        return {static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h)};
    }
    constexpr Rect<float> tof() const {
    return {
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(w),
        static_cast<float>(h)
    };
    }
    constexpr Rect<double> tod() const {
        return {
            static_cast<double>(x),
            static_cast<double>(y),
            static_cast<double>(w),
            static_cast<double>(h)
        };
    }
};
}

using Recti = detail::Rect<int>;
using Rectf = detail::Rect<float>;
using Rectd = detail::Rect<double>;
using Rectu = detail::Rect<unsigned>;
