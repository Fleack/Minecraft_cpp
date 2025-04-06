#pragma once

#include <string>
#include <GLFW/glfw3.h>

namespace mc::core
{
class Window
{
public:
    Window(std::string const& title, int width, int height);
    ~Window();

    void pollEvents();
    void swapBuffers();
    [[nodiscard]] bool isOpen() const;

    GLFWwindow* getNativeWindow();

private:
    bool initGLFW();
    bool initGLAD();

private:
    GLFWwindow* m_window = nullptr;
};
}
