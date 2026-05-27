#pragma once

#include <cmath>
#include <numbers>

namespace math
{

inline constexpr float kPi = std::numbers::pi_v<float>;

inline float deg_to_rad(float deg) noexcept
{
    return deg * (kPi / 180.0f);
}

inline float rad_to_deg(float rad) noexcept
{
    return rad * (180.0f / kPi);
}

inline float fix_angle(float a) noexcept
{
    a = std::fmod(a, 360.0f);
    if (a < 0.0f)
        a += 360.0f;
    return a;
}

} // namespace math
