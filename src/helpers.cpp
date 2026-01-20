#include "helpers.h"

namespace gfx
{
  float degToRad(const float a) noexcept { return a * (kPi / 180.0f); }

  float fixAngleDeg(float a) noexcept
  {
    while(a < 0.0f)
      a += 360.0f;
    while(a >= 360.0f)
      a -= 360.0f;
    return a;
  }
}
