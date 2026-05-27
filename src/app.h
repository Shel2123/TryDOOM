#pragma once

#include <memory>
#include <SDL3/SDL.h>
#include "renderer.h"
#include "input.h"
#include "player.h"

class App
{
public:
    App() = default;
    ~App();

    App(const App &) = delete;
    App &operator=(const App &) = delete;

    [[nodiscard]] bool init();
    void run();

private:
    void process_events();
    void update(float dt);
    void render() const;
    void update_framebuffer_size();

    SDL_Window *window_ = nullptr;
    SDL_GLContext gl_ctx_ = nullptr;
    std::unique_ptr<Renderer2D> renderer_;

    Input input_;
    Player player_;

    int fb_w_ = 0;
    int fb_h_ = 0;
    bool fullscreen_ = false;
    bool running_ = false;

    int num_rays_ = 1000;

    bool show_fps_ = false;
    int fps_frames_ = 0;
    double fps_accum_ = 0.0;

    Uint64 last_counter_ = 0;
    double perf_freq_ = 0.0;

    static constexpr int kWidth = 1024;
    static constexpr int kHeight = 510;
    static constexpr auto kTitle = "Wolf3D on GPU";
};
