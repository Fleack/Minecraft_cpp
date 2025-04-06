#pragma once

#include <string>

#include <glad/gl.h>

#include "render/IShader.hpp"

namespace mc::render
{
class Shader : public IShader
{
public:
    Shader(std::string const& vertexPath, std::string const& fragmentPath);
    ~Shader() override;

    void bind() override;
    void unbind() override;

private:
    std::string readFile(const std::string& path) const;
    GLuint compileShader(GLenum type, const std::string& source) const;
    void checkCompileErrors(GLuint shader, const std::string& type) const;
};
}
