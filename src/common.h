#pragma once
#include <cmath>
#include "helpers.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 512
inline bool keyA = false, keyD = false, keyW = false, keyS = false,
            keyRight = false, keyLeft = false;
inline float deltaTime = 0.0f;
inline float lastTime = 0.0f;

struct Player
{
  float x = 0.0f, y = 0.0f;
  float a = 90.0f;

  float dx{}, dy{};

  Player() = default;

  Player(const float x_, const float y_, const float a = 90)
      : x(x_), y(y_), a(a)
  {
    dx = cos(gfx::degToRad(a));
    dy = -sin(gfx::degToRad(a));
  }
  void updateDir()
  {
    dx = cos(gfx::degToRad(a));
    dy = -sin(gfx::degToRad(a));
  }
};

struct Map {
    int x = 8;
    int y = 11;
    static constexpr int size = 88;
    int worldMap[size] = {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 1, 0, 0, 1,
        1, 0, 0, 1, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 1, 1, 1, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
    };
};

inline Player player{};
inline Map map{};
