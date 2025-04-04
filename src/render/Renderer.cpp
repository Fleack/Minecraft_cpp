#include "render/Renderer.hpp"

#include "render/Shader.hpp"

#include <glad/gl.h>

namespace mc::render
{
Renderer::Renderer()
{
    float vertices[] = {
        // x       y
        0.0f, 0.5f,
        0.5f, -0.5f,
        -0.5f, -0.5f
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    m_shader = std::make_unique<Shader>("shaders/basic.vert", "shaders/basic.frag");
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void Renderer::Draw() const
{
    m_shader->bind();
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
}
