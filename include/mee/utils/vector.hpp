#pragma once

#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stddef.h>

namespace detail {

template <typename T> struct Vector2 {
    T x{};
    T y{};

    constexpr Vector2() = default;
    constexpr Vector2(T x, T y) : x(x), y(y) {}
    constexpr Vector2(T v) : x(v), y(x) {}


    constexpr Vector2 operator-() const { return {-x, -y}; }

    constexpr Vector2 operator+(const Vector2 &other) const {
    return {x + other.x, y + other.y};
    }

    constexpr Vector2 operator-(const Vector2 &other) const {
    return {x - other.x, y - other.y};
    }

    constexpr Vector2 operator*(T scalar) const {
    return {x * scalar, y * scalar};
    }

    constexpr Vector2 operator/(T scalar) const {
    return {x / scalar, y / scalar};
    }

    constexpr Vector2 operator*(Vector2<T> other) const {
    return {x * other.x, y * other.y};
    }

    constexpr Vector2 &operator+=(const Vector2 &other) {
    x += other.x;
    y += other.y;
    return *this;
    }

    constexpr Vector2 &operator-=(const Vector2 &other) {
    x -= other.x;
    y -= other.y;
    return *this;
    }

    constexpr Vector2 &operator*=(T scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
    }

    constexpr Vector2 &operator/=(T scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
    }

    constexpr bool operator==(const Vector2 &other) const {
    return x == other.x && y == other.y;
    }

    constexpr bool operator!=(const Vector2 &other) const {
    return !(*this == other);
    }

    constexpr Vector2(const ::Vector2 &other)
      : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

    explicit constexpr operator ::Vector2() const {
    return {static_cast<float>(x), static_cast<float>(y)};
    }
    constexpr ::Vector2 toRaylib() const {
    return {static_cast<float>(x), static_cast<float>(y)};
    }
    constexpr Vector2<float> tof() const {
    return {
        static_cast<float>(x),
        static_cast<float>(y),
    };
    }
    constexpr Vector2<double> tod() const {
    return {static_cast<double>(x), static_cast<double>(y)};
    }
    T length() const;
    T lengthSquared() const;
    Vector2 normalized() const;
    void normalize();

    static Vector2<T> zero();
    static Vector2<T> one();
    static T dot(const Vector2 &, const Vector2 &);
    static T cross(const Vector2 &, const Vector2 &);
    static T fromRaylib(const ::Vector2 &);

    T &operator[](size_t);
    const T &operator[](size_t) const;
    };

    }
    template <typename T>
    constexpr detail::Vector2<T> operator*(T scalar, const detail::Vector2<T> &v) {
    return {v.x * scalar, v.y * scalar};
    }

    using Vector2i = detail::Vector2<int>;
    using Vector2u = detail::Vector2<unsigned>;
    using Vector2f = detail::Vector2<float>;
    using Vector2d = detail::Vector2<double>;

    template <typename T> T detail::Vector2<T>::length() const {
    return std::sqrt(x * x + y * y);
    }

    template <typename T> T detail::Vector2<T>::lengthSquared() const {
    return x * x + y * y;
    }

    template <typename T>
    detail::Vector2<T> detail::Vector2<T>::normalized() const {
    T len = length();

    if (len == T{})
    return {T{}, T{}};

    return {x / len, y / len};
    }

    template <typename T> void detail::Vector2<T>::normalize() {
    T len = length();

    if (len == T{})
    return;

    x /= len;
    y /= len;
}

template <typename T>
T detail::Vector2<T>::dot(const Vector2 &a, const Vector2 &b) {
  return a.x * b.x + a.y * b.y;
}

template <typename T>
T detail::Vector2<T>::cross(const Vector2 &a, const Vector2 &b) {
  return a.x * b.y - a.y * b.x;
}

template <typename T> T &detail::Vector2<T>::operator[](size_t index) {
  assert(index < 2);

  return index == 0 ? x : y;
}

template <typename T>
const T &detail::Vector2<T>::operator[](size_t index) const {
  assert(index < 2);

  return index == 0 ? x : y;
}

template <typename T> detail::Vector2<T> detail::Vector2<T>::zero() {
  return {static_cast<T>(0), static_cast<T>(0)};
}

template <typename T> detail::Vector2<T> detail::Vector2<T>::one() {
  return {static_cast<T>(1), static_cast<T>(1)};
}

template <typename T> T detail::Vector2<T>::fromRaylib(const ::Vector2 &vec) {
  return {static_cast<float>(vec.x), static_cast<float>(vec.y)};
}
