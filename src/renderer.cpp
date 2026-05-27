#include "renderer.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iterator>

namespace
{

GLuint compile_shader(GLenum type, const char *src)
{
    const GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[2048];
        glGetShaderInfoLog(s, static_cast<GLsizei>(sizeof(log)), nullptr, log);
        std::fprintf(stderr, "Shader compile error:\n%s\n", log);
        std::abort();
    }
    return s;
}

GLuint link_program(GLuint vs, GLuint fs)
{
    const GLuint p = glCreateProgram();
    glAttachShader(p, vs);
    glAttachShader(p, fs);
    glLinkProgram(p);

    GLint ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[2048];
        glGetProgramInfoLog(p, static_cast<GLsizei>(sizeof(log)), nullptr, log);
        std::fprintf(stderr, "Program link error:\n%s\n", log);
        std::abort();
    }
    glDetachShader(p, vs);
    glDetachShader(p, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return p;
}

void setup_vao(GLuint &vao, GLuint &vbo)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2D),
                          reinterpret_cast<void *>(2 * sizeof(float)));

    glBindVertexArray(0);
}

} // namespace

Renderer2D::~Renderer2D()
{
    if (vbo_line_) glDeleteBuffers(1, &vbo_line_);
    if (vao_line_) glDeleteVertexArrays(1, &vao_line_);
    if (vbo_tri_) glDeleteBuffers(1, &vbo_tri_);
    if (vao_tri_) glDeleteVertexArrays(1, &vao_tri_);
    if (prog_) glDeleteProgram(prog_);
}

void Renderer2D::init()
{
    constexpr auto vs_src = R"GLSL(
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

    constexpr auto fs_src = R"GLSL(
        #version 330 core
        in vec3 vColor;
        out vec4 FragColor;
        void main() { FragColor = vec4(vColor, 1.0); }
    )GLSL";

    const GLuint vs = compile_shader(GL_VERTEX_SHADER, vs_src);
    const GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fs_src);
    prog_ = link_program(vs, fs);
    mvp_loc_ = glGetUniformLocation(prog_, "uMVP");

    setup_vao(vao_tri_, vbo_tri_);
    setup_vao(vao_line_, vbo_line_);

    glDisable(GL_DEPTH_TEST);
}

void Renderer2D::begin_frame(int w, int h)
{
    tris_.clear();
    lines_.clear();
    viewport_w_ = w;
    viewport_h_ = h;

    const auto fw = static_cast<float>(w);
    const auto fh = static_cast<float>(h);

    std::ranges::fill(mvp_, 0.0f);
    mvp_[0] = 2.0f / fw;
    mvp_[5] = -2.0f / fh;
    mvp_[10] = -1.0f;
    mvp_[12] = -1.0f;
    mvp_[13] = 1.0f;
    mvp_[15] = 1.0f;
}

void Renderer2D::push_quad(float x0, float y0, float x1, float y1,
                           float r, float g, float b)
{
    const Vertex2D a{x0, y0, r, g, b};
    const Vertex2D bb{x1, y0, r, g, b};
    const Vertex2D c{x1, y1, r, g, b};
    const Vertex2D d{x0, y1, r, g, b};
    tris_.push_back(a);
    tris_.push_back(bb);
    tris_.push_back(c);
    tris_.push_back(a);
    tris_.push_back(c);
    tris_.push_back(d);
}

void Renderer2D::push_line(float x0, float y0, float x1, float y1,
                           float r, float g, float b)
{
    lines_.push_back(Vertex2D{x0, y0, r, g, b});
    lines_.push_back(Vertex2D{x1, y1, r, g, b});
}

void Renderer2D::flush() const
{
    glViewport(0, 0, viewport_w_, viewport_h_);
    glUseProgram(prog_);
    glUniformMatrix4fv(mvp_loc_, 1, GL_FALSE, mvp_);

    if (!tris_.empty())
    {
        glBindVertexArray(vao_tri_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_tri_);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(tris_.size() * sizeof(Vertex2D)),
                     tris_.data(), GL_STREAM_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(tris_.size()));
    }

    if (!lines_.empty())
    {
        glBindVertexArray(vao_line_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_line_);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(lines_.size() * sizeof(Vertex2D)),
                     lines_.data(), GL_STREAM_DRAW);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lines_.size()));
    }

    glBindVertexArray(0);
    glUseProgram(0);
}
