#pragma once
#include "scene.h"

class PauseScene final : public Scene
{
public:
  PauseScene(Services &s, SceneStack &stack) : Scene(s, stack) {}

  [[nodiscard]] bool isOverlay() const override
  { return true; } // render game below
  void update(float dt) override;
  void render() const override;

private:
  int selected_ = 0; // 0 = Resume, 1 = Map select
};
