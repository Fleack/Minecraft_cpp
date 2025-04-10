#include <glad/gl.h>

#include "core/Window.hpp"
#include "core/Logger.hpp"

namespace mc::core
{
Window::Window(const std::string& title, int width, int height)
{
    if (!initGlfw()) return;

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        Logger::get()->error("Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1); // Enable vsync

    if (!initGlad())
    {
        Logger::get()->error("Failed to initialize GLAD");
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        return;
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
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

bool Window::initGlfw() const
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
    return true;
}

bool Window::initGlad() const
{
    if (!gladLoadGL(glfwGetProcAddress))
    {
        Logger::get()->error("Failed to initialize GLAD!");
        return false;
    }
    Logger::get()->info("OpenGL loaded: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    return true;
}

void Window::pollEvents() const
{
    glfwPollEvents();
}

void Window::swapBuffers() const
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
