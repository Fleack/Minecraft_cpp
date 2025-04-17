#include <glad/gl.h>

#include "core/Window.hpp"
#include "core/Logger.hpp"

namespace mc::core
{
Window::Window(const std::string& title, int width, int height)
{
    if (!initGlfw())
    {
        LOG(ERROR, "GLFW initialization failed");
        return;
    }

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        LOG(ERROR, "Failed to create GLFW window");
        glfwTerminate();
        return;
    }
    LOG(INFO, "GLFW window created with size: {}x{}", width, height);

    glfwMakeContextCurrent(m_window);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1); // Enable vsync

    if (!initGlad())
    {
        LOG(ERROR, "Failed to initialize GLAD");
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        return;
    }
    LOG(INFO, "OpenGL context loaded: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
}

bool Window::initGlfw() const
{
    if (!glfwInit())
    {
        LOG(ERROR, "Failed to initialize GLFW");
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
        LOG(ERROR, "Failed to initialize GLAD!");
        return false;
    }
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
