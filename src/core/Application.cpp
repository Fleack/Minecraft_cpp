#include "core/Application.hpp"

#include "core/CrashReporter.hpp"
#include "core/Logger.hpp"
#include "ecs/Ecs.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "ecs/component/ColliderComponent.hpp"
#include "ecs/component/PlayerComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/component/VelocityComponent.hpp"
#include "ecs/system/CameraSystem.hpp"
#include "ecs/system/ChunkLoadingSystem.hpp"
#include "ecs/system/PlayerInputSystem.hpp"
#include "ecs/system/RenderSystem.hpp"
#include "world/World.hpp"

#include <chrono>

#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Format.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Ui/Application.h>

namespace mc::core
{
using namespace Magnum;
using namespace Magnum::Platform;

Application::Application(Arguments const& arguments)
    : Sdl2Application{
          arguments,
          Configuration{}.setTitle("Minecraft cpp").setSize({1280, 720}),
          GLConfiguration{}.setVersion(GL::Version::GL460)}
    , m_chunkExecutor{m_runtime.thread_pool_executor()}
    , m_meshExecutor{m_runtime.thread_pool_executor()}
    , m_mainExecutor{m_runtime.make_manual_executor()}
    , m_aspectRatio{static_cast<float>(windowSize().x()) / windowSize().y()}
{
    initializeSystems();

    setCursor(Cursor::Hidden);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::setClearColor(Color4{0.5f, 0.7f, 1.0f, 1.0f});

    setSwapInterval(1);
    redraw();
}

void Application::drawEvent()
{
    using clock = std::chrono::steady_clock;
    static clock::time_point last = clock::now();
    auto now = clock::now();
    float deltaTime = std::chrono::duration<float>(now - last).count();
    last = now;

    if (auto pending = m_mainExecutor->size(); pending > 0)
        m_mainExecutor->loop(pending);

    GL::defaultFramebuffer.clear(
        GL::FramebufferClear::Color |
        GL::FramebufferClear::Depth);

    if (!m_paused)
    {
        m_ecs->update(deltaTime);
    }
    m_ecs->render(deltaTime);

    swapBuffers();
    redraw();
}

void Application::viewportEvent(ViewportEvent& event)
{
    m_aspectRatio = static_cast<float>(event.windowSize().x()) / event.windowSize().y();
    m_cameraSystem->setAspectRatio(m_aspectRatio);
    m_uiSystem->setWindowSize(event.windowSize());
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
}

void Application::keyPressEvent(KeyEvent& event)
{
    if (event.key() == Key::Esc)
    {
        m_paused = !m_paused;
        setCursor(m_paused ? Cursor::Arrow : Cursor::Hidden);
        SDL_SetRelativeMouseMode(m_paused ? SDL_FALSE : SDL_TRUE);
    }

    if (!m_paused)
    {
        m_playerInputSystem->handleKey(event.key(), true);
    }
    event.setAccepted();
}

void Application::keyReleaseEvent(KeyEvent& event)
{
    if (!m_paused)
    {
        m_playerInputSystem->handleKey(event.key(), false);
    }
    event.setAccepted();
}

void Application::pointerMoveEvent(Magnum::Platform::Sdl2Application::PointerMoveEvent& event)
{
    if (m_paused)
    {
        m_uiSystem->pointerMoveEvent(event);
    }
    else
    {
        m_cameraSystem->handleMouse(event.relativePosition());
        event.setAccepted();
    }
}

void Application::pointerPressEvent(Magnum::Platform::Sdl2Application::PointerEvent& event)
{
    if (!m_paused)
    {
        m_uiSystem->pointerPressEvent(event);
    }
    event.setAccepted();
}

void Application::pointerReleaseEvent(Magnum::Platform::Sdl2Application::PointerEvent& event)
{
    if (!m_paused)
    {
        m_uiSystem->pointerReleaseEvent(event);
    }
    event.setAccepted();
}

void Application::scrollEvent(ScrollEvent& event)
{
    if (!m_paused)
    {
        m_playerInputSystem->handleScroll(event.offset().y());
    }
    event.setAccepted();
}

void Application::initializeSystems()
{
    Logger::init();
    CrashReporter::init();
    m_ecs = std::make_unique<ecs::Ecs>();

    auto player = m_ecs->createEntity();
    m_ecs->addComponent<ecs::TransformComponent>(player, {.position = {0.0, 70.0, 0.0}});
    m_ecs->addComponent<ecs::VelocityComponent>(player, {});
    m_ecs->addComponent<ecs::PlayerComponent>(player, {});
    m_ecs->addComponent<ecs::CameraComponent>(player, {});
    m_ecs->addComponent<ecs::ColliderComponent>(player, {});

    m_world = std::make_unique<world::World>(m_chunkExecutor, m_ecs->eventBus());
    LOG(INFO, "Initialized world with seed={}", m_world->getSeed());

    m_playerInputSystem = std::make_shared<ecs::PlayerInputSystem>(*m_ecs);
    m_chunkLoadingSystem = std::make_shared<ecs::ChunkLoadingSystem>(*m_ecs, *m_world, RENDER_DISTANCE);
    m_gravitySystem = std::make_shared<ecs::GravitySystem>(*m_ecs);
    m_jumpSystem = std::make_shared<ecs::JumpSystem>(*m_ecs);
    m_collisionSystem = std::make_shared<ecs::CollisionSystem>(*m_ecs, *m_world);
    m_movementSystem = std::make_shared<ecs::MovementSystem>(*m_ecs);
    m_cameraSystem = std::make_shared<ecs::CameraSystem>(*m_ecs, m_aspectRatio);
    m_renderSystem = std::make_shared<ecs::RenderSystem>(*m_ecs, m_meshExecutor, m_cameraSystem, *m_world, RENDER_DISTANCE);
    m_uiSystem = std::make_shared<ecs::UISystem>(*m_ecs, *m_world, windowSize());

    m_ecs->addSystem(m_playerInputSystem);
    m_ecs->addSystem(m_chunkLoadingSystem);
    m_ecs->addSystem(m_gravitySystem);
    m_ecs->addSystem(m_jumpSystem);
    m_ecs->addSystem(m_collisionSystem);
    m_ecs->addSystem(m_movementSystem);
    m_ecs->addSystem(m_cameraSystem);
    m_ecs->addSystem(m_renderSystem);
    m_ecs->addSystem(m_uiSystem);
}

void Application::shutdown()
{
    m_renderSystem.reset();
    m_cameraSystem.reset();
    m_chunkLoadingSystem.reset();
    m_world.reset();
    m_ecs.reset();
}

} // namespace mc::core
