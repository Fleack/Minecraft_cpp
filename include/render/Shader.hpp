#pragma once

#include <string>

#include <glad/gl.h>

namespace mc::render
{
class Shader
{
public:
    Shader(std::string const& vertexPath, std::string const& fragmentPath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    [[nodiscard]] GLuint GetID() const { return m_id; }

private:
    std::string ReadFile(const std::string& path) const;
    GLuint CompileShader(GLenum type, const std::string& source) const;
    void CheckCompileErrors(GLuint shader, const std::string& type) const;

private:
    GLuint m_id;
};
}
