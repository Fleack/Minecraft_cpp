#pragma once

#include "IInputProvider.hpp"

#include <GLFW/glfw3.h>

namespace mc::input
{
class GLFWInputProvider final : public IInputProvider
{
public:
    explicit GLFWInputProvider(GLFWwindow& window)
        : m_window(window)
    {
    }

    bool isKeyPressed(int key) const override
    {
        return glfwGetKey(&m_window, key) == GLFW_PRESS;
    }

    glm::dvec2 getCursorPosition() const override
    {
        double x, y;
        glfwGetCursorPos(&m_window, &x, &y);
        return {x, y};
    }

private:
    GLFWwindow& m_window;
};
}
