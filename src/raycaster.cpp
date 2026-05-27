#include "raycaster.h"
#include "renderer.h"
#include "player.h"
#include "map.h"
#include "math_utils.h"

#include <algorithm>
#include <cfloat>
#include <climits>
#include <cmath>

namespace
{

constexpr auto kCellF = static_cast<float>(Map::kCellSize);

struct RayHit
{
    float x = 0.0f;
    float y = 0.0f;
    float dist = FLT_MAX;
    bool vertical = false;
};

struct RayStep
{
    int dof = 0;
    float rx = 0.0f, ry = 0.0f;
    float xo = 0.0f, yo = 0.0f;
};

RayStep init_horizontal(float ra_rad, float px, float py)
{
    RayStep s{};
    const float sin_a = std::sin(ra_rad);
    const float cos_a = std::cos(ra_rad);

    if (std::fabs(sin_a) < 1e-6f)
    {
        s.rx = px;
        s.ry = py;
        s.dof = INT_MAX;
        return s;
    }

    if (sin_a > 0.0f)
    {
        constexpr float kEps = 0.0001f;
        s.ry = std::floor(py / kCellF) * kCellF - kEps;
        const float t = (py - s.ry) / sin_a;
        s.rx = px + cos_a * t;
        s.yo = -kCellF;
    }
    else
    {
        s.ry = std::floor(py / kCellF) * kCellF + kCellF;
        const float t = (py - s.ry) / sin_a;
        s.rx = px + cos_a * t;
        s.yo = kCellF;
    }

    s.xo = -(cos_a / sin_a) * s.yo;
    s.dof = 0;
    return s;
}

RayStep init_vertical(float ra_rad, float px, float py)
{
    RayStep s{};
    const float sin_a = std::sin(ra_rad);
    const float cos_a = std::cos(ra_rad);

    if (std::fabs(cos_a) < 1e-6f)
    {
        s.rx = px;
        s.ry = py;
        s.dof = INT_MAX;
        return s;
    }

    if (cos_a > 0.0f)
    {
        s.rx = std::floor(px / kCellF) * kCellF + kCellF;
        const float t = (s.rx - px) / cos_a;
        s.ry = py - sin_a * t;
        s.xo = kCellF;
    }
    else
    {
        constexpr float kEps = 0.0001f;
        s.rx = std::floor(px / kCellF) * kCellF - kEps;
        const float t = (s.rx - px) / cos_a;
        s.ry = py - sin_a * t;
        s.xo = -kCellF;
    }

    s.yo = -(sin_a / cos_a) * s.xo;
    s.dof = 0;
    return s;
}

RayHit march_to_wall(RayStep s, float px, float py)
{
    constexpr int kMaxDof = Map::kWidth + Map::kHeight;
    RayHit hit;
    while (s.dof < kMaxDof)
    {
        const auto mx = static_cast<int>(s.rx / kCellF);
        const auto my = static_cast<int>(s.ry / kCellF);

        if (Map::is_wall(mx, my))
        {
            hit.x = s.rx;
            hit.y = s.ry;
            hit.dist = std::hypot(hit.x - px, hit.y - py);
            return hit;
        }

        s.rx += s.xo;
        s.ry += s.yo;
        ++s.dof;
    }

    return hit;
}

RayHit cast_ray(float ra_deg, float px, float py)
{
    ra_deg = math::fix_angle(ra_deg);
    const float ra_rad = math::deg_to_rad(ra_deg);

    const RayHit hh = march_to_wall(init_horizontal(ra_rad, px, py), px, py);

    RayHit vh = march_to_wall(init_vertical(ra_rad, px, py), px, py);
    vh.vertical = true;

    if (vh.dist <= hh.dist)
        return vh;
    return hh;
}

float corrected_distance(const Player &player, const RayHit &hit)
{
    const float d = std::cos(math::deg_to_rad(player.angle)) * (hit.x - player.x)
                    - std::sin(math::deg_to_rad(player.angle)) * (hit.y - player.y);
    return std::max(d, 0.0001f);
}

float proj_plane_dist(const Viewport &v, float fov_deg)
{
    return static_cast<float>(v.w) * 0.5f / std::tan(math::deg_to_rad(fov_deg * 0.5f));
}

} // namespace

void cast_and_draw(Renderer2D &renderer, const Player &player,
                   const Viewport &view, const int num_rays, bool draw_debug_rays)
{
    constexpr float kFovDeg = 90.0f;

    const auto vx0 = static_cast<float>(view.x0);
    const auto vy0 = static_cast<float>(view.y0);
    const auto vx1 = static_cast<float>(view.x0 + view.w);
    const float vy_mid = vy0 + static_cast<float>(view.h) * 0.5f;
    const auto vy1 = static_cast<float>(view.y0 + view.h);

    renderer.push_quad(vx0, vy0, vx1, vy_mid, 0.0f, 1.0f, 1.0f);
    renderer.push_quad(vx0, vy_mid, vx1, vy1, 0.0f, 0.0f, 1.0f);

    const float pp = proj_plane_dist(view, kFovDeg);
    const float col_w = static_cast<float>(view.w) / static_cast<float>(num_rays);

    for (int r = 0; r < num_rays; ++r)
    {
        const float screen_x = (static_cast<float>(r) + 0.5f) * col_w
                               - static_cast<float>(view.w) * 0.5f;
        const float ra = math::fix_angle(
            player.angle - math::rad_to_deg(std::atan(screen_x / pp)));

        const RayHit hit = cast_ray(ra, player.x, player.y);

        if (draw_debug_rays)
            renderer.push_line(player.x, player.y, hit.x, hit.y, 1.0f, 0.0f, 0.0f);

        const float d = corrected_distance(player, hit);
        float line_h = kCellF * pp / d;
        line_h = std::min(line_h, static_cast<float>(view.h));
        const float line_off = (static_cast<float>(view.h) - line_h) * 0.5f;

        constexpr float kFog = 0.00005f;
        const float shade = 1.0f / (1.0f + kFog * d * d);

        const float x0 = static_cast<float>(view.x0) + static_cast<float>(r) * col_w;
        const float x1 = x0 + col_w;
        const float y0 = static_cast<float>(view.y0) + line_off;
        const float y1 = y0 + line_h;

        renderer.push_quad(x0, y0, x1, y1, shade, shade, shade);
    }
}

void draw_minimap(Renderer2D &renderer)
{
    for (int y = 0; y < Map::kHeight; ++y)
    {
        for (int x = 0; x < Map::kWidth; ++x)
        {
            const float c = Map::is_wall(x, y) ? 1.0f : 0.0f;
            const auto xo = static_cast<float>(x) * kCellF;
            const auto yo = static_cast<float>(y) * kCellF;
            renderer.push_quad(xo + 1, yo + 1, xo + kCellF - 1, yo + kCellF - 1, c, c, c);
        }
    }
}

void draw_player_2d(Renderer2D &renderer, const Player &player)
{
    constexpr float kHalf = 4.0f;
    renderer.push_quad(player.x - kHalf, player.y - kHalf,
                       player.x + kHalf, player.y + kHalf,
                       1.0f, 1.0f, 0.0f);
    renderer.push_line(player.x, player.y,
                       player.x + player.dx * 20.0f, player.y + player.dy * 20.0f,
                       1.0f, 1.0f, 0.0f);
}
