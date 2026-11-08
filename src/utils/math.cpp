#include <mee/utils/math.hpp>
#include <math.h>
#include <numbers>

constexpr double DEG2RAD = std::numbers::pi / 180.0;
constexpr float DEG2RADF = std::numbers::pi / 180.0f;

float degToRadf(float deg){
    return deg * DEG2RADF;
}

double degToRad(double deg){
    return deg * DEG2RAD;
}
