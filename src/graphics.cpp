#include "graphics.h"
#include "common.h"

#include <algorithm>
#include <cmath>
#include "renderer2D.h"

#include <ostream>

namespace gfx
{
  RayStep initHorizontal(const float raRad, const float px, const float py)
  {
    RayStep s{};
    const float sinA = sin(raRad);
    const float cosA = cos(raRad);
    constexpr auto cell = static_cast<float>(Map::size);

    // ray perfectly horizontal -> no horizontal grid intersections
    if(fabs(sinA) < 1e-6f)
      {
        s.rx = px;
        s.ry = py;
        s.dof = INT_MAX; // makes marchToWall skip
        return s;
      }

    if(sinA > 0.0f) // looking up (y decreases)
      {
        constexpr float eps = 0.0001f;
        s.ry = floor(py / cell) * cell - eps;
        const float t = (py - s.ry) / sinA;
        s.rx = px + cosA * t;
        s.yo = -cell;
      }
    else // looking down (y increases)
      {
        s.ry = floor(py / cell) * cell + cell;
        const float t = (py - s.ry) / sinA;
        s.rx = px + cosA * t;
        s.yo = cell;
      }

    // x step per horizontal grid step
    s.xo = -(cosA / sinA) * s.yo;
    s.dof = 0;
    return s;
  }

  RayStep initVertical(const float raRad, const float px, const float py)
  {
    RayStep s{};
    const float sinA = sin(raRad);
    const float cosA = cos(raRad);
    constexpr auto cell = static_cast<float>(Map::size);

    // Ray perfectly vertical -> no vertical grid intersections
    if(fabs(cosA) < 1e-6f)
      {
        s.rx = px;
        s.ry = py;
        s.dof = INT_MAX; // makes marchToWall skip
        return s;
      }

    if(cosA > 0.0f) // looking right (x increases)
      {
        s.rx = floor(px / cell) * cell + cell;
        const float t = (s.rx - px) / cosA;
        s.ry = py - sinA * t;
        s.xo = cell;
      }
    else // looking left (x decreases)
      {
        constexpr float eps = 0.0001f;
        s.rx = floor(px / cell) * cell - eps;
        const float t = (s.rx - px) / cosA;
        s.ry = py - sinA * t;
        s.xo = -cell;
      }

    // y step per vertical grid step
    s.yo = -(sinA / cosA) * s.xo;
    s.dof = 0;
    return s;
  }
  static bool isWallCell(const Map &map, const int mx, const int my)
  {
    if(mx < 0 || my < 0 || mx >= map.x || my >= map.y)
      return true;
    return map.worldMap[my * map.x + mx] == 1;
  }

  static float
  distanceAlongView(const float ax, const float ay, const float bx,
                    const float by, const float angDeg)
  {
    return cos(degToRad(angDeg)) * (bx - ax)
           - sin(degToRad(angDeg)) * (by - ay);
  }

  static void drawBackground(const Viewport &v)
  {
    const auto x0 = static_cast<float>(v.x0);
    const auto y0 = static_cast<float>(v.y0);
    const auto x1 = static_cast<float>(v.x0 + v.w);
    const float yMid = y0 + static_cast<float>(v.h) * 0.5f;
    const auto y1 = static_cast<float>(v.y0 + v.h);

    // top half
    gRenderer2D->pushQuad(x0, y0, x1, yMid, 0.f, 1.f, 1.f);

    // bottom half
    gRenderer2D->pushQuad(x0, yMid, x1, y1, 0.f, 0.f, 1.f);
  }

  static float rayStepDeg(const float fovDeg, const int numRays)
  { return fovDeg / static_cast<float>(numRays); }

  static float columnWidth(const Viewport &v, const int numRays)
  { return static_cast<float>(v.w) / static_cast<float>(numRays); }

  static float projPlaneDist(const Viewport &v, const float fovDeg)
  { return (static_cast<float>(v.w) * 0.5f) / tan(degToRad(fovDeg * 0.5f)); }

  static RayHit marchToWall(const Map &map, RayStep s, const int maxDof,
                            const float px, const float py)
  {
    RayHit hit;

    while(s.dof < maxDof)
      {
        const int mx = static_cast<int>(s.rx / Map::size);

        if(const int my = static_cast<int>(s.ry / Map::size);
           isWallCell(map, mx, my))
          {
            hit.x = s.rx;
            hit.y = s.ry;
            hit.dist = hypot(hit.x - px, hit.y - py);
            return hit;
          }

        s.rx += s.xo;
        s.ry += s.yo;
        s.dof++;
      }

    return hit; // dist=FLT_MAX means "not found"
  }

  static RayHit chooseNearest(const RayHit &h, const RayHit &v)
  { return v.dist < h.dist ? v : h; }

  static RayHit castRay(const Map &map, const Player &player, float raDeg)
  {
    raDeg = fixAngleDeg(raDeg);
    const float raRad = degToRad(raDeg);

    const float px = player.x;
    const float py = player.y;
    const int maxDof = map.x + map.y;

    const RayStep hs = initHorizontal(raRad, px, py);
    const RayHit hh = marchToWall(map, hs, maxDof, px, py);

    const RayStep vs = initVertical(raRad, px, py);
    RayHit vh = marchToWall(map, vs, maxDof, px, py);
    vh.vertical = true;

    const bool useVertical = vh.dist <= hh.dist;
    RayHit best = useVertical ? vh : hh;
    best.vertical = useVertical;
    return best;
  }

  static void drawRay2D(const Player &player, const RayHit &hit)
  { gRenderer2D->pushLine(player.x, player.y, hit.x, hit.y, 1.f, 0.f, 0.f); }

  static float correctedDistance(const Player &player, const RayHit &hit)
  {
    const float d
      = distanceAlongView(player.x, player.y, hit.x, hit.y, player.a);
    return std::max(d, 0.0001f);
  }

  static void
  drawColumn3D(const Viewport &v, const int r, const int numRays,
               const float lineH, const float lineOff, const float d)
  {
    constexpr float k = 0.00005f;
    const float shade = 1.0f / (1.0f + k * d * d);

    const float colW = columnWidth(v, numRays);
    const float x0 = static_cast<float>(v.x0) + static_cast<float>(r) * colW;
    const float x1 = x0 + colW;

    const float y0 = static_cast<float>(v.y0) + lineOff;
    const float y1 = y0 + lineH;

    gRenderer2D->pushQuad(x0, y0, x1, y1, shade, shade, shade);
  }

  static void projectAndDrawColumn(const Viewport &v, const float fovDeg,
                                   const int r, const int numRays,
                                   const Player &player, const RayHit &hit)
  {
    const float pp = projPlaneDist(v, fovDeg);
    const float d = correctedDistance(player, hit);

    float lineH = Map::size * pp / d;
    lineH = std::min(lineH, static_cast<float>(v.h));

    const float lineOff = (static_cast<float>(v.h) * 0.5f) - (lineH * 0.5f);

    drawColumn3D(v, r, numRays, lineH, lineOff, d);
  }

  void Raycaster::draw(const Map &map, const Player &player) const
  {
    const auto &[fovDeg, numRays, view, drawDebugRays] = settings_;
    drawBackground(view);

    const float pp = projPlaneDist(view, fovDeg);
    const float colW = columnWidth(view, numRays);

    for(int r = 0; r < numRays; ++r)
      {
        // screenX: margin form the center of column to the center of screen
        const float screenX = (static_cast<float>(r) + 0.5f) * colW
                              - static_cast<float>(view.w) * 0.5f;

        // ray angle for this column
        const float ra = fixAngleDeg(player.a - radToDeg(atan(screenX / pp)));

        const RayHit hit = castRay(map, player, ra);

        if(drawDebugRays)
          drawRay2D(player, hit);

        projectAndDrawColumn(view, fovDeg, r, numRays, player, hit);
      }
  }

}

void drawRays2D(const gfx::Viewport &view, const bool drawDebugRays)
{
  static gfx::Raycaster rc{};

  auto s = rc.settings(); // copy prev settings
  s.view = view;          // set viewpoint
  s.drawDebugRays = drawDebugRays;
  rc.setSettings(s);

  rc.draw(map, player);
}

// old api
void drawRays2D()
{
  constexpr gfx::Viewport defaultView{}; // x0=526,y0=0,w=480,h=320
  drawRays2D(defaultView, true);
}
