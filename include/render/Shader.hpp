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

    void bind() const;
    void unbind() const;

    [[nodiscard]] GLuint getID() const { return m_id; }

private:
    std::string readFile(const std::string& path) const;
    GLuint compileShader(GLenum type, const std::string& source) const;
    void checkCompileErrors(GLuint shader, const std::string& type) const;

private:
    GLuint m_id;
};
}
