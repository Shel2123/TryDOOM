#include "common.h"

#include <GL/glut.h>

void KeyDown(const unsigned char key, int x, int y)
{
  if(key == 'a')
    keyA = true;
  if(key == 'd')
    keyD = true;
  if(key == 'w')
    keyW = true;
  if(key == 's')
    keyS = true;
}

void KeyUp(const unsigned char key, int x, int y)
{
  if(key == 'a')
    keyA = false;
  if(key == 'd')
    keyD = false;
  if(key == 'w')
    keyW = false;
  if(key == 's')
    keyS = false;
}

void SpecialDown(const int key, int x, int y)
{
  if(key == GLUT_KEY_LEFT)
    keyLeft = true;
  if(key == GLUT_KEY_RIGHT)
    keyRight = true;
}

void SpecialUp(const int key, int x, int y)
{
  if(key == GLUT_KEY_LEFT)
    keyLeft = false;
  if(key == GLUT_KEY_RIGHT)
    keyRight = false;
}

void Update(const float dt)
{
  constexpr float rotSpeed = 120.0f;
  constexpr float moveSpeed = 120.0f;

  if(keyLeft)
    {
      player.a += rotSpeed * dt;
      player.a = gfx::FixAng(player.a);
    }
  if(keyRight)
    {
      player.a -= rotSpeed * dt;
      player.a = gfx::FixAng(player.a);
    }
  if(keyRight || keyLeft)
    player.updateDir();

  const float fx = player.dx, fy = player.dy;
  const float rx = -player.dy, ry = player.dx;

  if(keyW)
    {
      player.x += fx * moveSpeed * dt;
      player.y += fy * moveSpeed * dt;
    }
  if(keyS)
    {
      player.x -= fx * moveSpeed * dt;
      player.y -= fy * moveSpeed * dt;
    }
  if(keyD)
    {
      player.x += rx * moveSpeed * dt;
      player.y += ry * moveSpeed * dt;
    }
  if(keyA)
    {
      player.x -= rx * moveSpeed * dt;
      player.y -= ry * moveSpeed * dt;
    }
}