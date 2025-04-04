#pragma once

#include <glad/gl.h>

#include <memory>

namespace mc::render
{
class Shader;

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void Draw() const;

private:
    GLuint m_vao;
    GLuint m_vbo;
    std::unique_ptr<Shader> m_shader;
};
}
