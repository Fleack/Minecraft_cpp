#pragma once

#include "ecs/Ecs.hpp"
#include "ecs/system/ISystem.hpp"
#include "world/World.hpp"

#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Ui/Label.h>
#include <Magnum/Ui/UserInterfaceGL.h>

namespace mc::ecs
{
// ReSharper disable once CppInconsistentNaming
class UISystem final : public ISystem
{
public:
    UISystem(Ecs&, world::World const& world, Magnum::Vector2i windowSize);

    void update(float) override {};
    void render(float deltaTime) override;

    bool pointerMoveEvent(Magnum::Platform::Sdl2Application::PointerMoveEvent& event);
    bool pointerPressEvent(Magnum::Platform::Sdl2Application::PointerEvent& event);
    bool pointerReleaseEvent(Magnum::Platform::Sdl2Application::PointerEvent& event);

    void setWindowSize(Magnum::Vector2i windowSize);

private:
    ecs::Ecs& m_ecs;
    world::World const& m_world;

    Magnum::Ui::UserInterfaceGL m_ui;
    Magnum::Ui::Label m_fpsLabel;
    Magnum::Ui::Label m_coordsLabel;
    Magnum::Ui::Label m_chunkLabel;
    Magnum::Ui::Label m_chunksCountLabel;
    Magnum::Ui::Label m_viewLabel;
    Magnum::Ui::Label m_speedLabel;
    Magnum::Ui::Label m_fovLabel;
};
} // namespace mc::ecs
