#pragma once

#include <array>
#include <cstddef>

struct Map
{
    static constexpr int kWidth = 8;
    static constexpr int kHeight = 11;
    static constexpr int kCellSize = 88;

    static constexpr std::array<int, kWidth * kHeight> kTiles = {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 1, 0, 0, 1,
        1, 0, 0, 1, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 1, 1, 1,
        1, 1, 1, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 1, 1, 1, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
    };

    [[nodiscard]] static constexpr bool is_wall(int mx, int my) noexcept
    {
        if (mx < 0 || my < 0 || mx >= kWidth || my >= kHeight)
            return true;
        return kTiles[static_cast<std::size_t>(my) * kWidth + static_cast<std::size_t>(mx)] == 1;
    }
};
