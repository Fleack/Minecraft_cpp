#pragma once

namespace mc::render
{
/**
 * @brief Abstract shader interface.
 *
 * Defines a base interface for managing GPU shader programs.
 */
class IShader
{
public:
    virtual ~IShader() = default;

    /**
     * @brief Activates (binds) the shader for rendering.
     */
    virtual void bind() = 0;

    /**
     * @brief Deactivates (unbinds) the shader.
     */
    virtual void unbind() = 0;

    /**
     * @brief Gets the OpenGL shader program ID.
     *
     * @return GLuint representing the shader program ID.
     */
    [[nodiscard]] GLuint getId() const { return m_id; }

protected:
    GLuint m_id{0}; ///< OpenGL shader program ID.
};
}
