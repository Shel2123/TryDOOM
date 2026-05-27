#pragma once

#include <SDL3/SDL.h>
#include <array>
#include <cstdint>

class Input
{
public:
    void update() noexcept
    {
        prev_ = curr_;
        const bool *ks = SDL_GetKeyboardState(nullptr);
        for (int i = 0; i < SDL_SCANCODE_COUNT; ++i)
            curr_[static_cast<std::size_t>(i)] = ks[i] ? 1 : 0;
    }

    [[nodiscard]] bool down(const SDL_Scancode sc) const noexcept
    {
        return curr_[sc] != 0;
    }

    [[nodiscard]] bool pressed(const SDL_Scancode sc) const noexcept
    {
        return curr_[sc] != 0 && prev_[sc] == 0;
    }

    [[nodiscard]] bool released(const SDL_Scancode sc) const noexcept
    {
        return curr_[sc] == 0 && prev_[sc] != 0;
    }

private:
    std::array<std::uint8_t, SDL_SCANCODE_COUNT> curr_{};
    std::array<std::uint8_t, SDL_SCANCODE_COUNT> prev_{};
};
