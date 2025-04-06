#pragma once
#include <glm/glm.hpp>

namespace mc::input
{
class IInputProvider
{
public:
    virtual ~IInputProvider() = default;

    virtual bool isKeyPressed(int key) const = 0;
    virtual glm::dvec2 getCursorPosition() const = 0;
};
}
