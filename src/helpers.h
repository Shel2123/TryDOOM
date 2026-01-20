#pragma once

namespace gfx
{
  constexpr float kPi = 3.14159265358979323846f;

  float degToRad(float a) noexcept;
  float fixAngleDeg(float a) noexcept;

  inline float FixAng(const float a) noexcept { return fixAngleDeg(a); }
}
