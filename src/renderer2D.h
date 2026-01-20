#pragma once
#include <vector>
#include <cstdio>

#include <glad/glad.h>

struct Vertex2D
{
  float x, y;
  float r, g, b;
};

struct Mat4
{
  float m[16]{};
};

inline Mat4 ortho(const float l, const float r, const float b, const float t)
{
  // Column-major
  Mat4 o{};
  o.m[0] = 2.0f / (r - l);
  o.m[5] = 2.0f / (t - b);
  o.m[10] = -1.0f;
  o.m[12] = -(r + l) / (r - l);
  o.m[13] = -(t + b) / (t - b);
  o.m[15] = 1.0f;
  return o;
}

inline GLuint compileShader(const GLenum type, const char *src)
{
  const GLuint s = glCreateShader(type);
  glShaderSource(s, 1, &src, nullptr);
  glCompileShader(s);

  GLint ok = 0;
  glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
  if(!ok)
    {
      char log[2048];
      glGetShaderInfoLog(s, static_cast<GLsizei>(sizeof(log)), nullptr, log);
      std::fprintf(stderr, "Shader compile failed:\n%s\n", log);
      std::abort();
    }
  return s;
}

inline GLuint linkProgram(const GLuint vs, const GLuint fs)
{
  const GLuint p = glCreateProgram();
  glAttachShader(p, vs);
  glAttachShader(p, fs);
  glLinkProgram(p);

  GLint ok = 0;
  glGetProgramiv(p, GL_LINK_STATUS, &ok);
  if(!ok)
    {
      char log[2048];
      glGetProgramInfoLog(p, static_cast<GLsizei>(sizeof(log)), nullptr, log);
      std::fprintf(stderr, "Program link failed:\n%s\n", log);
      std::abort();
    }
  glDetachShader(p, vs);
  glDetachShader(p, fs);
  glDeleteShader(vs);
  glDeleteShader(fs);
  return p;
}

class Renderer2D
{
public:
  void init()
  {
    const auto vsSrc = R"GLSL(
            #version 330 core
            layout (location = 0) in vec2 aPos;
            layout (location = 1) in vec3 aColor;
            uniform mat4 uMVP;
            out vec3 vColor;
            void main() {
                vColor = aColor;
                gl_Position = uMVP * vec4(aPos, 0.0, 1.0);
            }
        )GLSL";

    const auto fsSrc = R"GLSL(
            #version 330 core
            in vec3 vColor;
            out vec4 FragColor;
            void main() { FragColor = vec4(vColor, 1.0); }
        )GLSL";

    const GLuint vs = compileShader(GL_VERTEX_SHADER, vsSrc);
    const GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSrc);
    prog_ = linkProgram(vs, fs);
    uMVP_ = glGetUniformLocation(prog_, "uMVP");

    // Triangles
    glGenVertexArrays(1, &vaoTri_);
    glGenBuffers(1, &vboTri_);
    glBindVertexArray(vaoTri_);
    glBindBuffer(GL_ARRAY_BUFFER, vboTri_);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D),
                          static_cast<void *>(nullptr));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2D),
                          reinterpret_cast<void *>(2 * sizeof(float)));

    // Lines
    glGenVertexArrays(1, &vaoLine_);
    glGenBuffers(1, &vboLine_);
    glBindVertexArray(vaoLine_);
    glBindBuffer(GL_ARRAY_BUFFER, vboLine_);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D),
                          static_cast<void *>(nullptr));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2D),
                          reinterpret_cast<void *>(2 * sizeof(float)));

    glBindVertexArray(0);

    glDisable(GL_DEPTH_TEST);
  }

  void beginFrame(const int w, const int h)
  {
    tri_.clear();
    line_.clear();
    viewportW_ = w;
    viewportH_ = h;

    // Match old glOrtho(0, 1024, 510, 0, ...)
    // -> origin top-left, +y downward.
    mvp_ = ortho(0.0f, static_cast<float>(w), static_cast<float>(h), 0.0f);
  }

  void pushQuad(const float x0, const float y0, const float x1, const float y1,
                const float r, const float g, const float b)
  {
    // Two triangles (a,b,c) (a,c,d)
    const Vertex2D a{x0, y0, r, g, b};
    const Vertex2D bb{x1, y0, r, g, b};
    const Vertex2D c{x1, y1, r, g, b};
    const Vertex2D d{x0, y1, r, g, b};
    tri_.push_back(a);
    tri_.push_back(bb);
    tri_.push_back(c);
    tri_.push_back(a);
    tri_.push_back(c);
    tri_.push_back(d);
  }

  void pushLine(const float x0, const float y0, const float x1, const float y1,
                const float r, const float g, const float b)
  {
    line_.push_back(Vertex2D{x0, y0, r, g, b});
    line_.push_back(Vertex2D{x1, y1, r, g, b});
  }

  void flush() const
  {
    glViewport(0, 0, viewportW_, viewportH_);
    glUseProgram(prog_);
    glUniformMatrix4fv(uMVP_, 1, GL_FALSE, mvp_.m);

    // Draw triangles
    glBindVertexArray(vaoTri_);
    glBindBuffer(GL_ARRAY_BUFFER, vboTri_);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(tri_.size() * sizeof(Vertex2D)),
                 tri_.data(), GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(tri_.size()));

    // Draw lines
    glBindVertexArray(vaoLine_);
    glBindBuffer(GL_ARRAY_BUFFER, vboLine_);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(line_.size() * sizeof(Vertex2D)),
                 line_.data(), GL_STREAM_DRAW);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(line_.size()));

    glBindVertexArray(0);
    glUseProgram(0);
  }

private:
  GLuint prog_ = 0;
  GLint uMVP_ = -1;

  GLuint vaoTri_ = 0, vboTri_ = 0;
  GLuint vaoLine_ = 0, vboLine_ = 0;

  std::vector<Vertex2D> tri_;
  std::vector<Vertex2D> line_;

  int viewportW_ = 0, viewportH_ = 0;
  Mat4 mvp_{};
};

// glob
inline Renderer2D *gRenderer2D = nullptr;
