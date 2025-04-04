#pragma once

#include <string>
#include <GLFW/glfw3.h>

namespace mc::core
{
class Window
{
public:
    Window(const std::string& title, int width, int height);
    ~Window();

    void PollEvents();
    void SwapBuffers();
    [[nodiscard]] bool IsOpen() const;

private:
    bool InitGLFW();
    bool InitGLAD();

private:
    GLFWwindow* m_Window = nullptr;
};
}
