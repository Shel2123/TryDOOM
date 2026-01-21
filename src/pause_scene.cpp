#include "pause_scene.h"

#include "input.h"
#include "scene_stack.h"
#include "map_select_scene.h"
#include "renderer2D.h"

static void drawMenuBox(Renderer2D &r, const float x0, const float y0,
                        const float x1, const float y1, const bool selected)
{
  // no alpha in  shader right now, so just draw solid boxes.
  // selected -> brighter box.
  const float base = selected ? 0.85f : 0.65f;
  r.pushQuad(x0, y0, x1, y1, base, base, base);
}

void PauseScene::update(float)
{
  // esc -> resume (pop pause)
  if(svc_.input.pressed(SDL_SCANCODE_ESCAPE))
    {
      stack_.pop();
      return;
    }

  // navigate
  if(svc_.input.pressed(SDL_SCANCODE_UP) || svc_.input.pressed(SDL_SCANCODE_W))
    selected_ = (selected_ + 2 - 1) % 2;

  if(svc_.input.pressed(SDL_SCANCODE_DOWN)
     || svc_.input.pressed(SDL_SCANCODE_S))
    selected_ = (selected_ + 1) % 2;

  // activate
  if(svc_.input.pressed(SDL_SCANCODE_RETURN)
     || svc_.input.pressed(SDL_SCANCODE_SPACE))
    {
      if(selected_ == 0)
        {
          stack_.pop(); // resume
        }
      else
        {
          stack_.push(std::make_unique<MapSelectScene>(svc_, stack_));
        }
    }
}

void PauseScene::render() const
{
  auto &r = svc_.renderer;

  // Simple centered panel
  constexpr float w = 320.0f;
  constexpr float h = 200.0f;
  const float x0 = (static_cast<float>(svc_.fbW) - w) * 0.5f;
  const float y0 = (static_cast<float>(svc_.fbH) - h) * 0.5f;

  // panel
  r.pushQuad(x0, y0, x0 + w, y0 + h, 0.20f, 0.20f, 0.20f);

  // options
  constexpr float pad = 20.0f;
  constexpr float optH = 55.0f;

  drawMenuBox(r, x0 + pad, y0 + 60, x0 + w - pad, y0 + 60 + optH,
              selected_ == 0);
  drawMenuBox(r, x0 + pad, y0 + 125, x0 + w - pad, y0 + 125 + optH,
              selected_ == 1);
}
