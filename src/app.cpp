#include "app.h"
#include "raycaster.h"

#include <glad/glad.h>
#include <algorithm>
#include <cstdio>

namespace
{

void print_gl_info()
{
    auto safe_str = [](GLenum name) -> const char * {
        const auto *s = glGetString(name);
        return s ? reinterpret_cast<const char *>(s) : "<null>";
    };

    std::printf("GL_VENDOR   : %s\n", safe_str(GL_VENDOR));
    std::printf("GL_RENDERER : %s\n", safe_str(GL_RENDERER));
    std::printf("GL_VERSION  : %s\n", safe_str(GL_VERSION));
    std::printf("GLSL        : %s\n", safe_str(GL_SHADING_LANGUAGE_VERSION));
}

} // namespace

App::~App()
{
    renderer_.reset();

    if (gl_ctx_)
        SDL_GL_DestroyContext(gl_ctx_);
    if (window_)
        SDL_DestroyWindow(window_);

    SDL_Quit();
}

bool App::init()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_SetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES, "1");

    window_ = SDL_CreateWindow(kTitle, kWidth, kHeight,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window_)
    {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetWindowMinimumSize(window_, kWidth, kHeight);

    gl_ctx_ = SDL_GL_CreateContext(window_);
    if (!gl_ctx_)
    {
        std::fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        return false;
    }

    if (!SDL_GL_MakeCurrent(window_, gl_ctx_))
    {
        std::fprintf(stderr, "SDL_GL_MakeCurrent failed: %s\n", SDL_GetError());
        return false;
    }

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
    {
        std::fprintf(stderr, "Failed to init GLAD\n");
        return false;
    }

    update_framebuffer_size();
    SDL_GL_SetSwapInterval(1);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    renderer_ = std::make_unique<Renderer2D>();
    renderer_->init();

    perf_freq_ = static_cast<double>(SDL_GetPerformanceFrequency());
    last_counter_ = SDL_GetPerformanceCounter();

    print_gl_info();

    running_ = true;
    return true;
}

void App::run()
{
    while (running_)
    {
        process_events();

        const Uint64 now = SDL_GetPerformanceCounter();
        const auto dt = static_cast<float>(
            static_cast<double>(now - last_counter_) / perf_freq_);
        last_counter_ = now;

        update(dt);

        if (show_fps_)
        {
            fps_accum_ += dt;
            ++fps_frames_;
            if (fps_accum_ >= 0.25)
            {
                const double fps = fps_frames_ / fps_accum_;
                fps_accum_ = 0.0;
                fps_frames_ = 0;

                char title[256];
                std::snprintf(title, sizeof(title), "%s | FPS: %.1f", kTitle, fps);
                SDL_SetWindowTitle(window_, title);
            }
        }

        render();
        SDL_GL_SwapWindow(window_);
    }
}

void App::process_events()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_EVENT_QUIT)
        {
            running_ = false;
            return;
        }

        switch (e.type)
        {
        case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
        case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        case SDL_EVENT_WINDOW_MAXIMIZED:
        case SDL_EVENT_WINDOW_RESTORED:
        case SDL_EVENT_WINDOW_RESIZED:
            update_framebuffer_size();
            break;
        default:
            break;
        }
    }
}

void App::update(float dt)
{
    input_.update();

    if (input_.pressed(SDL_SCANCODE_ESCAPE))
    {
        running_ = false;
        return;
    }

    if (input_.pressed(SDL_SCANCODE_O))
    {
        show_fps_ = !show_fps_;
        if (!show_fps_)
            SDL_SetWindowTitle(window_, kTitle);
    }

    if (input_.pressed(SDL_SCANCODE_L))
    {
        const int step = num_rays_ < 6 ? 1 : num_rays_ < 51 ? 5 : 50;
        num_rays_ = std::clamp(num_rays_ + step, 1, 5000);
    }
    if (input_.pressed(SDL_SCANCODE_K))
    {
        const int step = num_rays_ <= 6 ? 1 : num_rays_ <= 51 ? 5 : 50;
        num_rays_ = std::clamp(num_rays_ - step, 1, 5000);
    }

    player_.update(input_, dt);
}

void App::render() const
{
    glClear(GL_COLOR_BUFFER_BIT);
    renderer_->begin_frame(fb_w_, fb_h_);

    if (!fullscreen_)
    {
        draw_minimap(*renderer_);
        draw_player_2d(*renderer_, player_);

        constexpr Viewport view_win{};
        cast_and_draw(*renderer_, player_, view_win, num_rays_, true);
    }
    else
    {
        const Viewport view_full{0, 0, fb_w_, fb_h_};
        cast_and_draw(*renderer_, player_, view_full, num_rays_, false);
    }

    renderer_->flush();
}

void App::update_framebuffer_size()
{
    SDL_GetWindowSizeInPixels(window_, &fb_w_, &fb_h_);

    if (fb_w_ <= 0 || fb_h_ <= 0)
        SDL_GetWindowSize(window_, &fb_w_, &fb_h_);

    if (fb_w_ <= 0 || fb_h_ <= 0)
    {
        fb_w_ = kWidth;
        fb_h_ = kHeight;
    }

    const auto flags = SDL_GetWindowFlags(window_);
    fullscreen_ = (flags & SDL_WINDOW_FULLSCREEN) != 0
                  || (flags & SDL_WINDOW_MAXIMIZED) != 0;
}
