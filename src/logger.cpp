#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstdio>

namespace Logger
{
  static const char *SafeGLString(const GLenum name)
  {
    const GLubyte *s = glGetString(name);
    return s ? reinterpret_cast<const char *>(s) : nullptr;
  }

  void PrintGLInfo()
  {
    if(!SDL_GL_GetCurrentContext())
      {
        std::fprintf(stderr, "No current OpenGL context!\n");
        return;
      }

    const char *vendor = SafeGLString(GL_VENDOR);
    const char *renderer = SafeGLString(GL_RENDERER);
    const char *version = SafeGLString(GL_VERSION);
    const char *glsl = SafeGLString(GL_SHADING_LANGUAGE_VERSION);

    std::printf("GL_VENDOR   : %s\n", vendor ? vendor : "<null>");
    std::printf("GL_RENDERER : %s\n", renderer ? renderer : "<null>");
    std::printf("GL_VERSION  : %s\n", version ? version : "<null>");
    std::printf("GLSL        : %s\n", glsl ? glsl : "<null>");

    if(const GLenum err = glGetError(); err != GL_NO_ERROR)
      std::fprintf(stderr, "glGetError() after PrintGLInfo: 0x%X\n", err);
  }
}
