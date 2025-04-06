#include <glad/gl.h>

#include "core/Window.hpp"
#include "core/Logger.hpp"

namespace mc::core
{
Window::Window(const std::string& title, int width, int height)
{
    if (!initGLFW()) return;

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        Logger::get()->error("Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);
    glfwMakeContextCurrent(m_window);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1); // Enable vsync

    if (!initGLAD())
    {
        Logger::get()->error("Failed to initialize GLAD");
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        return;
    }

    glViewport(0, 0, width, height);
    Logger::get()->info("Window created: {}x{}", width, height);
}

Window::~Window()
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}

bool Window::initGLFW()
{
    if (!glfwInit())
    {
        Logger::get()->error("Failed to initialize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glEnable(GL_DEPTH_TEST);
    return true;
}

bool Window::initGLAD()
{
    if (!gladLoadGL(glfwGetProcAddress))
    {
        Logger::get()->error("Failed to initialize GLAD!");
        return false;
    }
    Logger::get()->info("OpenGL loaded: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    return true;
}

void Window::pollEvents()
{
    glfwPollEvents();
}

void Window::swapBuffers()
{
    glfwSwapBuffers(m_window);
}

bool Window::isOpen() const
{
    return m_window && !glfwWindowShouldClose(m_window);
}

GLFWwindow* Window::getNativeWindow()
{
    return m_window;
}
}
