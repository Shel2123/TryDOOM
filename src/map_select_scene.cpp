#include "map_select_scene.h"

#include "scene_stack.h"
#include "game_scene.h"
#include "renderer2D.h"
#include "map.h"

static void drawBox(Renderer2D &r, const float x0, const float y0,
                    const float x1, const float y1, const bool sel)
{
  const float c = sel ? 0.80f : 0.55f;
  r.pushQuad(x0, y0, x1, y1, c, c, c);
}

static void drawPanel(Renderer2D &r, const float x0, const float y0,
                      const float x1, const float y1)
{ r.pushQuad(x0, y0, x1, y1, 0.18f, 0.18f, 0.18f); }

static void drawMapPreview(Renderer2D &r, const float x0, const float y0,
                           const float scale, const Maps::MapEntry &e)
{
  const int H = e.getHeight();
  const int W = e.getWidth();
  for(int y = 0; y < H; ++y)
    for(int x = 0; x < W; ++x)
      {
        const int idx = y * W + x;
        const int t = e.tiles[idx];

        const float px0 = x0 + static_cast<float>(x) * scale;
        const float py0 = y0 + static_cast<float>(y) * scale;
        const float px1 = px0 + scale - 1.0f;
        const float py1 = py0 + scale - 1.0f;

        if(t == 1)
          r.pushQuad(px0, py0, px1, py1, 0.85f, 0.85f, 0.85f); // wall
        else
          r.pushQuad(px0, py0, px1, py1, 0.30f, 0.30f, 0.30f); // floor
      }
}

void MapSelectScene::update(float)
{
  // esc -> back to pause (pop map select)
  if(svc_.input.pressed(SDL_SCANCODE_ESCAPE))
    {
      stack_.pop();
      return;
    }

  const int count = Maps::count();

  if(svc_.input.pressed(SDL_SCANCODE_UP) || svc_.input.pressed(SDL_SCANCODE_W))
    selected_ = (selected_ + count - 1) % count;

  if(svc_.input.pressed(SDL_SCANCODE_DOWN)
     || svc_.input.pressed(SDL_SCANCODE_S))
    selected_ = (selected_ + 1) % count;

  // enter/space -> apply selection and restart game
  if(svc_.input.pressed(SDL_SCANCODE_RETURN)
     || svc_.input.pressed(SDL_SCANCODE_SPACE))
    {
      ApplyMapSelection(Maps::kMaps[selected_]);

      stack_.clear();
      stack_.push(std::make_unique<GameScene>(svc_, stack_));
    }
}

void MapSelectScene::render() const
{
  auto &r = svc_.renderer;

  const auto W = static_cast<float>(svc_.fbW);
  const auto H = static_cast<float>(svc_.fbH);

  // big centered panel
  constexpr float panelW = 760.0f;
  constexpr float panelH = 360.0f;
  const float x0 = (W - panelW) * 0.5f;
  const float y0 = (H - panelH) * 0.5f;
  drawPanel(r, x0, y0, x0 + panelW, y0 + panelH);

  // left: list
  const float listX0 = x0 + 25.0f;
  const float listY0 = y0 + 40.0f;

  for(int i = 0; i < static_cast<int>(Maps::count()); ++i)
    {
      constexpr float listW = 360.0f;
      constexpr float rowH = 45.0f;
      const float ry0 = listY0 + static_cast<float>(i) * (rowH + 10.0f);
      const float ry1 = ry0 + rowH;
      drawBox(r, listX0, ry0, listX0 + listW, ry1, i == selected_);
    }

  // right: preview
  const float prevX0 = x0 + 430.0f;
  const float prevY0 = y0 + 60.0f;
  constexpr float scale = 22.0f;

  drawPanel(r, prevX0 - 10, prevY0 - 10, prevX0 + 8 * scale + 10,
            prevY0 + 11 * scale + 10);
  drawMapPreview(r, prevX0, prevY0, scale, Maps::kMaps[selected_]);
}
