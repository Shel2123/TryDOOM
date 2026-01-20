#include <glad/glad.h>
#include <cstdio>

#include "renderer2D.h"
#include "controls.h"
#include "player.h"
#include "map.h"
#include "graphics.h"
#include "logger.h"
#include "app.h"

bool App::init(const AppConfig &cfg)
{
  config = cfg;

  if(!SDL_Init(SDL_INIT_VIDEO))
    {
      std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
      return false;
    }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                      SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, config.glMajor);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, config.glMinor);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_SetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES, "1");

  window = SDL_CreateWindow(config.title, config.width, config.height,
                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

  if(!window)
    {
      std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
      return false;
    }

  SDL_SetWindowMinimumSize(window, config.width, config.height);


  ctx = SDL_GL_CreateContext(window);
  if(!ctx)
    {
      std::fprintf(stderr, "SDL_GL_CreateContext failed: %s\n",
                   SDL_GetError());
      return false;
    }

  if(!SDL_GL_MakeCurrent(window, ctx))
    {
      std::fprintf(stderr, "SDL_GL_MakeCurrent failed: %s\n", SDL_GetError());
      return false;
    }

  if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
    {
      std::fprintf(stderr, "Failed to init GLAD\n");
      return false;
    }
  refreshFramebufferSize();
  updateSizes();


  SDL_GL_SetSwapInterval(config.vsync ? 1 : 0);

  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

  // Game state init
  player.x = 150;
  player.y = 150;
  player.a = 90;
  player.updateDir();

  // Renderer init
  static Renderer2D r; // simplest owning option
  r.init();
  renderer = &r;
  gRenderer2D = renderer;

  freq = static_cast<double>(SDL_GetPerformanceFrequency());
  lastCounter = SDL_GetPerformanceCounter();

  Logger::PrintGLInfo();

  running = true;
  return true;
}

void App::run()
{
  while(running)
    {
      processEvents();
      pollInput();

      const float dt = computeDt();
      update(dt);
      render();

      SDL_GL_SwapWindow(window);
    }
}

void App::shutdown()
{
  if(ctx)
    {
      SDL_GL_DestroyContext(ctx);
      ctx = nullptr;
    }
  if(window)
    {
      SDL_DestroyWindow(window);
      window = nullptr;
    }
  SDL_Quit();
}

void App::processEvents()
{
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT) running = false;

      if (e.type == SDL_EVENT_WINDOW_ENTER_FULLSCREEN ||
          e.type == SDL_EVENT_WINDOW_LEAVE_FULLSCREEN ||
          e.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
          updateSizes();
          }
  }
}


void App::pollInput()
{
  const bool *ks = SDL_GetKeyboardState(nullptr);
  keyW = ks[SDL_SCANCODE_W];
  keyA = ks[SDL_SCANCODE_A];
  keyS = ks[SDL_SCANCODE_S];
  keyD = ks[SDL_SCANCODE_D];
  keyLeft = ks[SDL_SCANCODE_LEFT];
  keyRight = ks[SDL_SCANCODE_RIGHT];
}

float App::computeDt()
{
  const Uint64 now = SDL_GetPerformanceCounter();
  const auto dt = static_cast<float>((now - lastCounter) / freq);
  lastCounter = now;
  return dt;
}

void App::update(const float dt) { Update(dt); }

void App::render() const
{
  glClear(GL_COLOR_BUFFER_BIT);

  renderer->beginFrame(fbW_, fbH_);

  if (!fullscreen_) {
      drawMap2D();
      drawPlayer2D();

      gfx::Viewport viewWin{};          // default (526,0,480,320)
      drawRays2D(viewWin, true);        // debug rays ON
  } else {
      gfx::Viewport viewFull;
      viewFull.x0 = 0;
      viewFull.y0 = 0;
      viewFull.w  = fbW_;
      viewFull.h  = fbH_;

      drawRays2D(viewFull, false);      // debug rays OFF -> 3D only
  }

  renderer->flush();
}

void App::refreshFramebufferSize()
{
  SDL_GetWindowSizeInPixels(window, &fbW_, &fbH_);
  if (fbW_ <= 0 || fbH_ <= 0) {
      fbW_ = config.width;
      fbH_ = config.height;
  }
}

void App::updateSizes()
{
  SDL_GetWindowSizeInPixels(window, &fbW_, &fbH_);

  if (fbW_ <= 0 || fbH_ <= 0) {
      SDL_GetWindowSize(window, &fbW_, &fbH_);
  }
  if (fbW_ <= 0 || fbH_ <= 0) {
      fbW_ = config.width;
      fbH_ = config.height;
  }

  fullscreen_ = (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) != 0;
}


