#pragma once

#include "IInputProvider.hpp"

#include <GLFW/glfw3.h>

namespace mc::input
{
/**
 * @brief GLFW-based implementation of the IInputProvider interface.
 *
 * Uses a GLFW window to query keyboard and mouse input state.
 */
class GlfwInputProvider final : public IInputProvider
{
public:
    /**
     * @brief Constructs the input provider with a given GLFW window.
     *
     * @param window Reference to the GLFW window.
     */
    explicit GlfwInputProvider(GLFWwindow& window)
        : m_window(window)
    {
        glfwSetWindowUserPointer(&m_window, this);
        glfwSetScrollCallback(&m_window, [](GLFWwindow* w, double /*xoffset*/, double yoffset) {
            auto* self = static_cast<GlfwInputProvider*>(glfwGetWindowUserPointer(w));
            std::lock_guard lock(self->m_mutex);
            self->m_scrollY += yoffset;
        });
    }

    /**
     * @brief Checks if a specific key is currently pressed.
     *
     * @param key Key code to check (GLFW key code).
     * @return True if the key is pressed, false otherwise.
     */
    bool isKeyPressed(int key) const override
    {
        return glfwGetKey(&m_window, key) == GLFW_PRESS;
    }

    /**
     * @brief Gets the current cursor position in screen coordinates.
     *
     * @return 2D vector representing the cursor position.
     */
    glm::dvec2 getCursorPosition() const override
    {
        double x, y;
        glfwGetCursorPos(&m_window, &x, &y);
        return {x, y};
    }

    /**
     * @brief Gets delta of mouse wheel scroll
     *
     * @return A float representing delta between last and current position of mouse wheel
     */
    float getScrollDelta() const override
    {
        std::lock_guard lock(m_mutex);
        double v = m_scrollY;
        m_scrollY = 0.0;
        return v;
    }

private:
    GLFWwindow& m_window; ///< Reference to the GLFW window used for input queries.

    mutable double m_scrollY = 0.0;
    mutable std::mutex m_mutex;
};
} // namespace mc::input
