#pragma once
#include <functional>
#include "input.h"

class Renderer2D;
class Input;

struct Services
{
  Renderer2D &renderer;
  Input &input;

  int &fbW;
  int &fbH;
  bool &fullscreen;

  std::function<void()> requestQuit; // scenes can call this
};
