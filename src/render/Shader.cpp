#include "render/Shader.hpp"

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>

namespace mc::render
{
Shader::Shader(std::string const& vertexPath, std::string const& fragmentPath)
{
    std::string vertexCode = ReadFile(vertexPath);
    std::string fragmentCode = ReadFile(fragmentPath);

    GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);

    m_id = glCreateProgram();
    glAttachShader(m_id, vertex);
    glAttachShader(m_id, fragment);
    glLinkProgram(m_id);

    CheckCompileErrors(m_id, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

void Shader::Bind() const
{
    glUseProgram(m_id);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

std::string Shader::ReadFile(const std::string& path) const
{
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint Shader::CompileShader(GLenum type, const std::string& source) const
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    CheckCompileErrors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
    return shader;
}

void Shader::CheckCompileErrors(GLuint shader, const std::string& type) const
{
    GLint success;
    std::array<GLchar, 1024> infoLog;

    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog.data());
            std::cerr << "[SHADER COMPILE ERROR] " << type << ": " << infoLog.data() << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog.data());
            std::cerr << "[SHADER LINK ERROR] " << type << ": " << infoLog.data() << std::endl;
        }
    }
}
}
