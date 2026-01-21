#include "common.h"
#include "renderer2D.h"

void drawPlayer2D()
{
  // player "point" as a small quad (8x8-ish)
  constexpr float half = 4.0f;
  gRenderer2D->pushQuad(player.x - half, player.y - half, player.x + half,
                        player.y + half, 1.f, 1.f, 0.f);

  // direction line (thin; wide lines aren't reliable in core profile)
  gRenderer2D->pushLine(player.x, player.y, player.x + player.dx * 20.f,
                        player.y + player.dy * 20.f, 1.f, 1.f, 0.f);
}
