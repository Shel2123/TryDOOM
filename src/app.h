#pragma once
#include <SDL3/SDL.h>
#include <memory>

class Renderer2D;

#include "input.h"
#include "services.h"
#include "scene_stack.h"

struct AppConfig
{
  AppConfig() = default;
  AppConfig(const int width, const int height, const char *title,
            const bool vsync)
      : width(width), height(height), title(title), vsync(vsync)
  {}
  int width = 1024;
  int height = 510;
  const char *title = "Wolf3D on GPU";
  int glMajor = 3;
  int glMinor = 3;
  bool vsync = true;
};

class App
{
public:
  bool init(const AppConfig &cfg);
  void run();
  void shutdown();
  void switchFPSCounter();

private:
  void updateSizes();
  void processEvents();
  float computeDt();
  void render() const;
  void refreshFramebufferSize();
  void updateFPSCounter(double dt);

  AppConfig config{};
  SDL_Window *window = nullptr;
  SDL_GLContext ctx = nullptr;
  bool running = false;

  bool fullscreen_ = false;

  struct FPS
  {
    bool status = false;
    double fpsAccum_ = 0.0;
    int fpsFrames_ = 0;
    double fpsValue_ = 0.0;
  };
  FPS fpsCounter_;

  int fbW_ = 0;
  int fbH_ = 0;

  Uint64 lastCounter = 0;
  double freq = 0.0;

  Renderer2D *renderer = nullptr;

  Input input_;
  std::unique_ptr<Services> services_;
  std::unique_ptr<SceneStack> scenes_;
};
