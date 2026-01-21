#pragma once
#include "services.h"

class SceneStack;

class Scene
{
public:
  Scene(Services &s, SceneStack &stack) : svc_(s), stack_(stack) {}
  virtual ~Scene() = default;

  // lifecycle hooks
  virtual void onPushed() {}
  virtual void onPopped() {}
  virtual void onCovered() {}   // another scene pushed on top
  virtual void onUncovered() {} // becomes top again

  // composition
  [[nodiscard]] virtual bool isOverlay() const
  { return false; } // if true -> render below too
  [[nodiscard]] virtual bool blocksUpdateBelow() const { return true; }

  // game loop
  virtual void update(float dt) = 0;
  virtual void render() const = 0;

protected:
  Services &svc_;
  SceneStack &stack_;
};
