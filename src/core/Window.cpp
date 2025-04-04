#include <glad/gl.h>

#include "core/Window.hpp"
#include "core/Logger.hpp"

namespace mc::core
{
Window::Window(const std::string& title, int width, int height)
{
    if (!InitGLFW()) return;

    m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_Window)
    {
        Logger::GetLogger()->error("Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1); // Enable vsync

    if (!InitGLAD())
    {
        Logger::GetLogger()->error("Failed to initialize GLAD");
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
        return;
    }

    glViewport(0, 0, width, height);
    Logger::GetLogger()->info("Window created: {}x{}", width, height);
}

Window::~Window()
{
    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }
}

bool Window::InitGLFW()
{
    if (!glfwInit())
    {
        Logger::GetLogger()->error("Failed to initialize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    return true;
}

bool Window::InitGLAD()
{
    if (!gladLoadGL(glfwGetProcAddress))
    {
        return false;
    }
    Logger::GetLogger()->info("OpenGL loaded: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    return true;
}

void Window::PollEvents()
{
    glfwPollEvents();
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(m_Window);
}

bool Window::IsOpen() const
{
    return m_Window && !glfwWindowShouldClose(m_Window);
}
}
