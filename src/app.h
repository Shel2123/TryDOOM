#pragma once
#include <SDL3/SDL.h>

class Renderer2D;

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
  bool vsync = false;
};

class App
{
public:
  bool init(const AppConfig &cfg);
  void run();
  void shutdown();

private:
  void updateSizes();
  void processEvents();
  static void pollInput();
  float computeDt();
  static void update(float dt);
  void render() const;
  void refreshFramebufferSize();

  AppConfig config{};
  SDL_Window *window = nullptr;
  SDL_GLContext ctx = nullptr;
  bool running = false;

  bool fullscreen_ = false;

  int fbW_ = 0;
  int fbH_ = 0;

  Uint64 lastCounter = 0;
  double freq = 0.0;

  Renderer2D *renderer = nullptr;
};
