#pragma once
#include <memory>
#include <vector>
#include <utility>
#include "scene.h"

class SceneStack
{
public:
  using Ptr = std::unique_ptr<Scene>;

  explicit SceneStack(Services &s) : svc_(s) {}

  void commit() { applyOps_(); } // call once after initial push

  // public API
  void push(Ptr s) { ops_.push_back({Op::Push, std::move(s)}); }
  void pop() { ops_.push_back({Op::Pop, nullptr}); }
  void replaceTop(Ptr s) { ops_.push_back({Op::Replace, std::move(s)}); }
  void clear() { ops_.push_back({Op::Clear, nullptr}); }

  [[nodiscard]] bool empty() const { return stack_.empty(); }

  void update(const float dt)
  {
    applyOps_();
    if(stack_.empty())
      return;

    // only top updates by default
    // if it is ever needed to "update below" for some overlay, may be extended later
    stack_.back()->update(dt);

    applyOps_();
  }

  void render() const
  {
    if(stack_.empty())
      return;

    // find the first scene to render
    int start = static_cast<int>(stack_.size()) - 1;
    while(start > 0 && stack_[start]->isOverlay())
      --start;

    for(auto i = static_cast<size_t>(start); i < stack_.size(); ++i)
      stack_[i]->render();
  }

private:
  enum class Op
  {
    Push,
    Pop,
    Replace,
    Clear
  };
  struct PendingOp
  {
    Op op;
    Ptr scene;
  };

  void applyOps_()
  {
    for(auto &[op, scene] : ops_)
      {
        switch(op)
          {
          case Op::Push:
            if(!stack_.empty())
              stack_.back()->onCovered();
            stack_.push_back(std::move(scene));
            stack_.back()->onPushed();
            break;

          case Op::Pop:
            if(!stack_.empty())
              {
                stack_.back()->onPopped();
                stack_.pop_back();
                if(!stack_.empty())
                  stack_.back()->onUncovered();
                else
                  svc_.requestQuit(); // no scenes left -> quit
              }
            break;

          case Op::Replace:
            if(!stack_.empty())
              {
                stack_.back()->onPopped();
                stack_.pop_back();
              }
            stack_.push_back(std::move(scene));
            stack_.back()->onPushed();
            break;

          case Op::Clear:
            while(!stack_.empty())
              {
                stack_.back()->onPopped();
                stack_.pop_back();
              }
            break;
          }
      }
    ops_.clear();
  }

  Services &svc_;
  std::vector<Ptr> stack_;
  std::vector<PendingOp> ops_;
};
