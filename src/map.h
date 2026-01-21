#pragma once
#include <array>
#include <cstddef>
#include <vector>
#include <string_view>
#include <cassert>

namespace Maps
{
  struct PlayerSpawn
  {
    float x = 150.0f;
    float y = 150.0f;
    float a = 90.0f;
  };

  struct MapEntry
  {
    std::string_view name = "Unnamed";
    int width = 0;
    int height = 0;
    std::vector<int> tiles; // width * height
    PlayerSpawn spawn{};

    [[nodiscard]] int size() const { return width * height; }
    [[nodiscard]] int getWidth() const { return width; }
    [[nodiscard]] int getHeight() const { return height; }

    int &at(const int x, const int y)
    {
      assert(x >= 0 && x < width && y >= 0 && y < height);
      return tiles[y * width + x];
    }
    [[nodiscard]] const int &at(const int x, const int y) const
    {
      assert(x >= 0 && x < width && y >= 0 && y < height);
      return tiles[y * width + x];
    }
  };

  inline MapEntry
  MakeMap(const std::string_view name, const int w, const int h,
          std::vector<int> t, const PlayerSpawn sp = {})
  {
    assert(static_cast<int>(t.size()) == w * h);
    return MapEntry{name, w, h, std::move(t), sp};
  }
  inline const std::vector kMaps = {
    MakeMap("E1M1 (Default)", 8, 11,
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0,
             0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0,
             0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0,
             1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            PlayerSpawn{150, 150, 0}),

    MakeMap("Arena", 8, 11,
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1,
             0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0,
             0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0,
             0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            PlayerSpawn{220, 220, 45}),
    MakeMap("Penis", 11, 11,
            {
              1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0,
              0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0,
              0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            },
            PlayerSpawn{150, 150, 0}

            )};

  inline std::size_t count() { return kMaps.size(); }

  void drawMap2D();
  void ApplyMapSelection(const MapEntry &e);

}
