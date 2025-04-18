#include "render/Shader.hpp"

#include <fstream>
#include <sstream>

#include "core/Logger.hpp"

namespace mc::render
{
Shader::Shader(std::string const& vertexPath, std::string const& fragmentPath)
{
    LOG(INFO, "Loading vertex shader from: {}", vertexPath);
    LOG(INFO, "Loading fragment shader from: {}", fragmentPath);

    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    m_id = glCreateProgram();
    glAttachShader(m_id, vertex);
    glAttachShader(m_id, fragment);
    glLinkProgram(m_id);

    checkCompileErrors(m_id, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    LOG(INFO, "Shader program successfully created with ID: {}", m_id);
}

void Shader::bind()
{
    glUseProgram(m_id);
}

void Shader::unbind()
{
    glUseProgram(0);
}

std::string Shader::readFile(const std::string& path) const
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        LOG(ERROR, "Failed to open shader file: {}", path);
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();
    LOG(DEBUG, "Read shader file: {}", path);
    return ss.str();
}

GLuint Shader::compileShader(GLenum type, const std::string& source) const
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    checkCompileErrors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
    LOG(INFO, "Successfully compiled {} shader", type == GL_VERTEX_SHADER ? "vertex" : "fragment");
    return shader;
}

void Shader::checkCompileErrors(GLuint shader, const std::string& type) const
{
    GLint success;
    GLchar infoLog[1024];

    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            LOG(ERROR, "[SHADER COMPILE ERROR]: {}", infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            LOG(ERROR, "[PROGRAM LINK ERROR]: {}", infoLog);
        }
    }
}
} // namespace mc::render
