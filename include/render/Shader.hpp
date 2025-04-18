#pragma once

#include <string>

#include <glad/gl.h>

#include "render/IShader.hpp"

namespace mc::render
{
/**
 * @brief Concrete OpenGL shader implementation.
 *
 * Loads, compiles, and manages a vertex-fragment shader program.
 */
class Shader : public IShader
{
public:
    /**
     * @brief Constructs and compiles a shader program from source files.
     *
     * @param vertexPath Path to the vertex shader source file.
     * @param fragmentPath Path to the fragment shader source file.
     */
    Shader(std::string const& vertexPath, std::string const& fragmentPath);
    ~Shader() override = default;

    /**
     * @brief Activates the shader for rendering.
     */
    void bind() override;

    /**
     * @brief Deactivates the shader.
     */
    void unbind() override;

private:
    /**
     * @brief Reads shader source code from a file.
     *
     * @param path Path to the shader source file.
     * @return File content as a string.
     */
    std::string readFile(const std::string& path) const;

    /**
     * @brief Compiles a shader from source code.
     *
     * @param type Shader type (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, etc.).
     * @param source Shader source code.
     * @return Compiled shader ID.
     */
    GLuint compileShader(GLenum type, const std::string& source) const;

    /**
     * @brief Checks and logs shader compilation or linking errors.
     *
     * @param shader ID of the shader or program.
     * @param type Type name for error reporting ("VERTEX", "FRAGMENT", etc.).
     */
    void checkCompileErrors(GLuint shader, const std::string& type) const;
};
} // namespace mc::render
