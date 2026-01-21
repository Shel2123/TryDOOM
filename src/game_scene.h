#pragma once
#include "scene.h"

class GameScene final : public Scene
{
public:
  GameScene(Services &s, SceneStack &stack) : Scene(s, stack) {}

  void onPushed() override;
  void update(float dt) override;
  void render() const override;
};
