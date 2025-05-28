#pragma once

#include "ecs/Ecs.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "ecs/component/PlayerComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/component/VelocityComponent.hpp"
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
    void renderWithoutInterval();
    void renderWithInterval(float deltaTime);
    float calculatePercentile(float percentile);

private:
    ecs::Ecs& m_ecs;
    world::World const& m_world;

    // Labels
    Magnum::Ui::UserInterfaceGL m_ui;
    Magnum::Ui::Label m_fpsLabel;
    Magnum::Ui::Label m_coordsLabel;
    Magnum::Ui::Label m_chunkLabel;
    Magnum::Ui::Label m_chunksCountLabel;
    Magnum::Ui::Label m_viewLabel;
    Magnum::Ui::Label m_speedLabel;
    Magnum::Ui::Label m_fovLabel;
    Magnum::Ui::Label m_playerInfo;

    // Components
    TransformComponent& m_transformComponent;
    CameraComponent& m_cameraComponent;
    PlayerComponent& m_playerComponent;
    VelocityComponent& m_velocityComponent;

    // Refresh interval
    float m_timeSinceLastFpsUpdate = 0.0f;
    static constexpr float fpsUpdateInterval{0.5f};

    // FPS Relates
    std::deque<float> m_frameTimes;
    static constexpr std::size_t frameSampleSize{500};
};
} // namespace mc::ecs
