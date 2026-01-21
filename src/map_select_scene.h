#pragma once
#include "scene.h"

class MapSelectScene final : public Scene
{
public:
  MapSelectScene(Services &s, SceneStack &stack) : Scene(s, stack) {}

  void update(float dt) override;
  void render() const override;

private:
  int selected_ = 0; // placeholder
};
