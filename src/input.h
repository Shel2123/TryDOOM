#pragma once
#include <SDL3/SDL.h>
#include <array>

class Input
{
public:
  void update()
  {
    prev_ = curr_;
    const bool *ks = SDL_GetKeyboardState(nullptr);
    for(int i = 0; i < SDL_SCANCODE_COUNT; ++i)
      curr_[i] = static_cast<uint8_t>(ks[i] ? 1 : 0);
  }

  [[nodiscard]] bool down(const SDL_Scancode sc) const
  { return curr_[sc] != 0; }

  [[nodiscard]] bool pressed(const SDL_Scancode sc) const
  { return curr_[sc] != 0 && prev_[sc] == 0; }

  [[nodiscard]] bool released(const SDL_Scancode sc) const
  { return curr_[sc] == 0 && prev_[sc] != 0; }

private:
  std::array<uint8_t, SDL_SCANCODE_COUNT> curr_{};
  std::array<uint8_t, SDL_SCANCODE_COUNT> prev_{};
};
