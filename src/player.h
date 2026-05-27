#pragma once

#include "math_utils.h"
#include "input.h"
#include <cmath>

struct Player
{
    float x = 150.0f;
    float y = 150.0f;
    float angle = 90.0f;
    float dx = 0.0f;
    float dy = 0.0f;

    Player() noexcept { update_direction(); }

    void update_direction() noexcept
    {
        dx = std::cos(math::deg_to_rad(angle));
        dy = -std::sin(math::deg_to_rad(angle));
    }

    void update(const Input &input, const float dt) noexcept
    {
        constexpr float kRotSpeed = 120.0f;
        constexpr float kMoveSpeed = 120.0f;

        bool rotated = false;
        if (input.down(SDL_SCANCODE_LEFT))
        {
            angle = math::fix_angle(angle + kRotSpeed * dt);
            rotated = true;
        }
        if (input.down(SDL_SCANCODE_RIGHT))
        {
            angle = math::fix_angle(angle - kRotSpeed * dt);
            rotated = true;
        }
        if (rotated)
            update_direction();

        const float rx = -dy;
        const float ry = dx;

        if (input.down(SDL_SCANCODE_W)) { x += dx * kMoveSpeed * dt; y += dy * kMoveSpeed * dt; }
        if (input.down(SDL_SCANCODE_S)) { x -= dx * kMoveSpeed * dt; y -= dy * kMoveSpeed * dt; }
        if (input.down(SDL_SCANCODE_D)) { x += rx * kMoveSpeed * dt; y += ry * kMoveSpeed * dt; }
        if (input.down(SDL_SCANCODE_A)) { x -= rx * kMoveSpeed * dt; y -= ry * kMoveSpeed * dt; }
    }
};
