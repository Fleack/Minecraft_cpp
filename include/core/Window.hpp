#pragma once

#include <string>

#include <GLFW/glfw3.h>

namespace mc::core
{
/**
 * @brief Handles creation and management of the application window using GLFW.
 *
 * Encapsulates window setup, OpenGL context initialization, input mode configuration,
 * and buffer/event handling. Also initializes GLAD for OpenGL function loading.
 */
class Window
{
public:
    /**
     * @brief Constructs and initializes a window with the given parameters.
     *
     * Initializes GLFW and GLAD, creates an OpenGL context, enables VSync,
     * and configures depth testing and cursor behavior.
     *
     * @param title Title of the window.
     * @param width Width in pixels.
     * @param height Height in pixels.
     */
    Window(std::string const& title, int width, int height);
    ~Window() = default;

    /**
     * @brief Polls GLFW events (input, window resize, etc.).
     */
    void pollEvents() const;

    /**
     * @brief Swaps the front and back buffers.
     */
    void swapBuffers() const;

    /**
     * @brief Checks if the window is still open.
     *
     * @return True if the window should remain open, false otherwise.
     */
    [[nodiscard]] bool isOpen() const;

    /**
     * @brief Returns the native GLFW window pointer.
     *
     * Useful for integration with other libraries (e.g., ImGui).
     *
     * @return Pointer to the GLFW window.
     */
    GLFWwindow* getNativeWindow();

private:
    /**
     * @brief Initializes GLFW and sets OpenGL context hints.
     *
     * @return True if GLFW was initialized successfully.
     */
    bool initGlfw() const;

    /**
     * @brief Initializes GLAD to load OpenGL functions.
     *
     * @return True if GLAD initialized successfully.
     */
    bool initGlad() const;

private:
    GLFWwindow* m_window{nullptr}; ///< Pointer to the native GLFW window.
};
} // namespace mc::core
