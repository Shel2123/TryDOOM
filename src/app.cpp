#include <glad/glad.h>
#include <cstdio>

#include "renderer2D.h"
#include "logger.h"
#include "app.h"

#include "common.h"
#include "map_select_scene.h"

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

  // Renderer init
  static Renderer2D r; // simplest owning option
  r.init();
  renderer = &r;
  gRenderer2D = renderer;

  freq = static_cast<double>(SDL_GetPerformanceFrequency());
  lastCounter = SDL_GetPerformanceCounter();

  Logger::PrintGLInfo();

  running = true;

  // services + scene stack
  services_ = std::make_unique<Services>(
    Services{*renderer, input_, fbW_, fbH_, fullscreen_,
             [this]() { running = false; }});

  scenes_ = std::make_unique<SceneStack>(*services_);
  scenes_->push(std::make_unique<MapSelectScene>(*services_, *scenes_));
  scenes_->commit();

  return true;
}

void App::run()
{
  while(running)
    {
      processEvents();

      input_.update();

      // keep old global keys alive for existing Update(dt)
      keyW = input_.down(SDL_SCANCODE_W);
      keyA = input_.down(SDL_SCANCODE_A);
      keyS = input_.down(SDL_SCANCODE_S);
      keyD = input_.down(SDL_SCANCODE_D);
      keyLeft = input_.down(SDL_SCANCODE_LEFT);
      keyRight = input_.down(SDL_SCANCODE_RIGHT);
      keyO = input_.down(SDL_SCANCODE_O);

      const float dt = computeDt();

      scenes_->update(dt);

      if(fpsToggleRequested)
        {
          fpsToggleRequested = false;
          switchFPSCounter();
        }

      updateFPSCounter(dt);
      render();

      SDL_GL_SwapWindow(window);
    }
}

void App::render() const
{
  glClear(GL_COLOR_BUFFER_BIT);

  renderer->beginFrame(fbW_, fbH_);

  scenes_->render();

  renderer->flush();
}

void App::shutdown()
{
  scenes_.reset();
  services_.reset();

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
  while(SDL_PollEvent(&e))
    {
      if(e.type == SDL_EVENT_QUIT)
        running = false;

      if(e.type == SDL_EVENT_WINDOW_ENTER_FULLSCREEN
         || e.type == SDL_EVENT_WINDOW_LEAVE_FULLSCREEN
         || e.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED
         || e.type == SDL_EVENT_WINDOW_MAXIMIZED
         || e.type == SDL_EVENT_WINDOW_RESTORED
         || e.type == SDL_EVENT_WINDOW_RESIZED
         || e.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
        {
          updateSizes();
        }
    }
}

float App::computeDt()
{
  const Uint64 now = SDL_GetPerformanceCounter();
  const auto dt = static_cast<float>((now - lastCounter) / freq);
  lastCounter = now;
  return dt;
}

void App::refreshFramebufferSize()
{
  SDL_GetWindowSizeInPixels(window, &fbW_, &fbH_);
  if(fbW_ <= 0 || fbH_ <= 0)
    {
      fbW_ = config.width;
      fbH_ = config.height;
    }
}

void App::switchFPSCounter() { fpsCounter_.status = !fpsCounter_.status; }

void App::updateFPSCounter(const double dt)
{
  if(fpsCounter_.status)
    {
      fpsCounter_.fpsAccum_ += dt;
      fpsCounter_.fpsFrames_++;

      if(fpsCounter_.fpsAccum_ >= 0.25)
        {
          fpsCounter_.fpsValue_
            = fpsCounter_.fpsFrames_ / fpsCounter_.fpsAccum_;
          fpsCounter_.fpsAccum_ = 0.0;
          fpsCounter_.fpsFrames_ = 0;

          char title[256];
          std::snprintf(title, sizeof(title), "%s | FPS: %.1f", config.title,
                        fpsCounter_.fpsValue_);
          SDL_SetWindowTitle(window, title);
        }
    }
}

void App::updateSizes()
{
  SDL_GetWindowSizeInPixels(window, &fbW_, &fbH_);

  if(fbW_ <= 0 || fbH_ <= 0)
    SDL_GetWindowSize(window, &fbW_, &fbH_);

  if(fbW_ <= 0 || fbH_ <= 0)
    {
      fbW_ = config.width;
      fbH_ = config.height;
    }

  fullscreen_ = (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN
                 || SDL_GetWindowFlags(window) & SDL_WINDOW_MAXIMIZED)
                != 0;
}
