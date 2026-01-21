#include "game_scene.h"

#include "controls.h"
#include "common.h"
#include "graphics.h"
#include "map.h"
#include "player.h"
#include "scene_stack.h"

// forward declare pause scene
#include "pause_scene.h"

#include <SDL3/SDL_scancode.h>

void GameScene::onPushed()
{
  // old "Game state init" here
  player.x = 150;
  player.y = 150;
  player.a = 90;
  player.updateDir();
}

void GameScene::update(const float dt)
{
  // esc -> pause overlay
  if(svc_.input.pressed(SDL_SCANCODE_ESCAPE))
    {
      stack_.push(std::make_unique<PauseScene>(svc_, stack_));
      return;
    }

  Update(dt);
}

void GameScene::render() const
{
  if(!svc_.fullscreen)
    {
      Maps::drawMap2D();
      drawPlayer2D();

      constexpr gfx::Viewport viewWin{}; // default (526,0,480,320)
      drawRays2D(viewWin, true);         // debug rays ON
    }
  else
    {
      gfx::Viewport viewFull;
      viewFull.x0 = 0;
      viewFull.y0 = 0;
      viewFull.w = svc_.fbW;
      viewFull.h = svc_.fbH;

      drawRays2D(viewFull, false); // debug rays OFF -> 3D only
    }
}
