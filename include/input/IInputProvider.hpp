#pragma once
#include <glm/glm.hpp>

namespace mc::input
{
/**
 * @brief Interface for input handling.
 *
 * Provides abstract access to keyboard and mouse input state.
 */
class IInputProvider
{
public:
    virtual ~IInputProvider() = default;

    /**
     * @brief Checks if a specific key is currently pressed.
     *
     * @param key Key code to check.
     * @return True if the key is pressed, false otherwise.
     */
    virtual bool isKeyPressed(int key) const = 0;

    /**
     * @brief Gets the current cursor position in screen coordinates.
     *
     * @return 2D vector representing the cursor position.
     */
    virtual glm::dvec2 getCursorPosition() const = 0;
};
} // namespace mc::input
