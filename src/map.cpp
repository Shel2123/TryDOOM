#include "map.h"

#include "common.h"
#include "renderer2D.h"

namespace Maps
{
  void drawMap2D()
  {
    for(int y = 0; y < map.y; y++)
      for(int x = 0; x < map.x; x++)
        {
          const bool wall = (map.worldMap[y * map.x + x] == 1);
          const float r = wall ? 1.f : 0.f;
          const float g = wall ? 1.f : 0.f;
          const float b = wall ? 1.f : 0.f;

          const auto xo = static_cast<float>(x * Map::size);
          const auto yo = static_cast<float>(y * Map::size);

          // keep +1/-1 inset
          gRenderer2D->pushQuad(xo + 1, yo + 1, xo + Map::size - 1,
                                yo + Map::size - 1, r, g, b);
        }
  }
  void ApplyMapSelection(const MapEntry &e)
  {
    std::copy(e.tiles.begin(), e.tiles.end(), map.worldMap);

    player.x = e.spawn.x;
    player.y = e.spawn.y;
    player.a = e.spawn.a;
    player.updateDir();
  }
}
