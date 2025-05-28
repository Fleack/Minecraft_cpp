#pragma once

#include "ecs/Ecs.hpp"

#include <unordered_set>

#include <Magnum/Platform/Sdl2Application.h>

namespace mc::ecs
{

class PlayerInputSystem final : public ISystem
{
public:
    explicit PlayerInputSystem(Ecs& ecs);

    void update(float dt) override;
    void render(float) override {};

    void handleKey(Magnum::Platform::Sdl2Application::Key key, bool pressed);
    void handleScroll(float yOffset);

private:
    Ecs& m_ecs;
    std::unordered_set<Magnum::Platform::Sdl2Application::Key> m_keysPressed;
};

} // namespace mc::ecs
