#pragma once

#include <cfloat>
#include "common.h"

namespace gfx
{
  struct RayHit
  {
    float x = 0.0f;
    float y = 0.0f;
    float dist = FLT_MAX;
    bool vertical = false;
  };

  struct RayStep
  {
    float rx = 0.0f, ry = 0.0f;
    float xo = 0.0f, yo = 0.0f;
    int dof = 0;
  };

  RayStep initHorizontal(float raRad, float px, float py);
  RayStep initVertical(float raRad, float px, float py);

  struct Viewport
  {
    int x0 = 526;
    int y0 = 0;
    int w = 480;
    int h = 320;
  };

  class Raycaster
  {
  public:
    struct Settings
    {
      float fovDeg = 90.0f;
      int numRays = 640;
      Viewport view{};
      bool drawDebugRays = true;
    };

    Raycaster() = default;
    explicit Raycaster(const Settings &s) : settings_(s) {}

    void draw(const Map &map, const Player &player) const;

    [[nodiscard]] const Settings &settings() const noexcept
    { return settings_; }
    void setSettings(const Settings &s) { settings_ = s; }

  private:
    Settings settings_{};
  };

}

void drawRays2D(const gfx::Viewport& view, bool drawDebugRays);
