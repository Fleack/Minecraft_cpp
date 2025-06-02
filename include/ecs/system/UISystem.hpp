#pragma once

#include "ecs/Ecs.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "ecs/component/PlayerComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/component/VelocityComponent.hpp"
#include "ecs/system/ISystem.hpp"
#include "utils/FpsStats.hpp"
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
    struct LabelDef
    {
        std::string name;
        Magnum::Vector2 offset;
        float width;
        Magnum::Ui::Label* label;
    };

    UISystem(Ecs&, world::World const& world, Magnum::Vector2i windowSize);

    void update(float) override {};
    void render(float deltaTime) override;

    bool pointerMoveEvent(Magnum::Platform::Sdl2Application::PointerMoveEvent& event);
    bool pointerPressEvent(Magnum::Platform::Sdl2Application::PointerEvent& event);
    bool pointerReleaseEvent(Magnum::Platform::Sdl2Application::PointerEvent& event);

    void setWindowSize(Magnum::Vector2i windowSize);

private:
    void renderWithoutInterval();
    void renderWithInterval(float deltaTime);

private:
    ecs::Ecs& m_ecs;
    world::World const& m_world;

    // Labels
    Magnum::Ui::UserInterfaceGL m_ui;
    Magnum::Ui::Label m_fpsLabel{Corrade::NoCreate, m_ui};
    Magnum::Ui::Label m_coordsLabel{Corrade::NoCreate, m_ui};
    Magnum::Ui::Label m_chunkLabel{Corrade::NoCreate, m_ui};
    Magnum::Ui::Label m_chunksCountLabel{Corrade::NoCreate, m_ui};
    Magnum::Ui::Label m_viewLabel{Corrade::NoCreate, m_ui};
    Magnum::Ui::Label m_speedLabel{Corrade::NoCreate, m_ui};
    Magnum::Ui::Label m_fovLabel{Corrade::NoCreate, m_ui};
    Magnum::Ui::Label m_playerInfo{Corrade::NoCreate, m_ui};

    // Components
    TransformComponent& m_transformComponent;
    CameraComponent& m_cameraComponent;
    PlayerComponent& m_playerComponent;
    VelocityComponent& m_velocityComponent;

    // Refresh interval
    float m_timeSinceLastFpsUpdate = 0.0f;
    static constexpr float FPS_UPDATE_INTERVAL{0.5f};

    // FPS Relates
    FpsStats m_fpsStats;
};
} // namespace mc::ecs
