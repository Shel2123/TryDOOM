#pragma once

#include <vector>
#include <glad/glad.h>

struct Vertex2D
{
    float x, y;
    float r, g, b;
};

class Renderer2D
{
public:
    Renderer2D() = default;
    ~Renderer2D();

    Renderer2D(const Renderer2D &) = delete;
    Renderer2D &operator=(const Renderer2D &) = delete;
    Renderer2D(Renderer2D &&) = delete;
    Renderer2D &operator=(Renderer2D &&) = delete;

    void init();
    void begin_frame(int w, int h);
    void push_quad(float x0, float y0, float x1, float y1, float r, float g, float b);
    void push_line(float x0, float y0, float x1, float y1, float r, float g, float b);
    void flush() const;

private:
    GLuint prog_ = 0;
    GLint mvp_loc_ = -1;

    GLuint vao_tri_ = 0, vbo_tri_ = 0;
    GLuint vao_line_ = 0, vbo_line_ = 0;

    std::vector<Vertex2D> tris_;
    std::vector<Vertex2D> lines_;

    int viewport_w_ = 0, viewport_h_ = 0;
    float mvp_[16]{};
};
