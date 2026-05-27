#pragma once

class Renderer2D;
struct Player;

struct Viewport
{
    int x0 = 526;
    int y0 = 0;
    int w = 480;
    int h = 320;
};

void cast_and_draw(Renderer2D &renderer, const Player &player,
                   const Viewport &view, int num_rays, bool draw_debug_rays);
void draw_minimap(Renderer2D &renderer);
void draw_player_2d(Renderer2D &renderer, const Player &player);
