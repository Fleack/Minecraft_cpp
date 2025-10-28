#include "core/ClientApplication.hpp"

#include "core/Logger.hpp"
#include "ecs/Ecs.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include <ecs/component/PlayerComponent.hpp>
#include <ecs/component/VelocityComponent.hpp>
#include "systems/CameraSystem.hpp"
#include "systems/PlayerInputSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/UISystem.hpp"
#include "world/SimpleChunkProvider.hpp"

#include <chrono>

#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Format.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Vector2.h>

namespace mc::core
{
using namespace Magnum;
using namespace Magnum::Platform;

ClientApplication::ClientApplication(Arguments const& arguments)
    : Sdl2Application{
          arguments,
          Configuration{}.setTitle("Minecraft cpp").setSize({1280, 720}),
          GLConfiguration{}.setVersion(GL::Version::GL460)}
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

void ClientApplication::drawEvent()
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

void ClientApplication::viewportEvent(ViewportEvent& event)
{
    m_aspectRatio = static_cast<float>(event.windowSize().x()) / event.windowSize().y();
    m_cameraSystem->setAspectRatio(m_aspectRatio);
    m_uiSystem->setWindowSize(event.windowSize());
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
}

void ClientApplication::keyPressEvent(KeyEvent& event)
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

void ClientApplication::keyReleaseEvent(KeyEvent& event)
{
    if (!m_paused)
    {
        m_playerInputSystem->handleKey(event.key(), false);
    }
    event.setAccepted();
}

void ClientApplication::pointerMoveEvent(Magnum::Platform::Sdl2Application::PointerMoveEvent& event)
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

void ClientApplication::pointerPressEvent(Magnum::Platform::Sdl2Application::PointerEvent& event)
{
    if (!m_paused)
    {
        m_uiSystem->pointerPressEvent(event);
    }
    event.setAccepted();
}

void ClientApplication::pointerReleaseEvent(Magnum::Platform::Sdl2Application::PointerEvent& event)
{
    if (!m_paused)
    {
        m_uiSystem->pointerReleaseEvent(event);
    }
    event.setAccepted();
}

void ClientApplication::scrollEvent(ScrollEvent& event)
{
    if (!m_paused)
    {
        m_playerInputSystem->handleScroll(event.offset().y());
    }
    event.setAccepted();
}

void ClientApplication::initializeSystems()
{
    Logger::init();
    m_ecs = std::make_unique<ecs::Ecs>();

    // TODO: Create player entity when receiving from server
    auto player = m_ecs->createEntity();
    m_ecs->addComponent<ecs::TransformComponent>(player, {.position = {0.0, 70.0, 0.0}});
    m_ecs->addComponent<ecs::CameraComponent>(player, {});
    m_ecs->addComponent<ecs::PlayerComponent>(player, {});
    m_ecs->addComponent<ecs::VelocityComponent>(player, {});

    // Initialize chunk provider
    m_chunkProvider = std::make_unique<world::SimpleChunkProvider>();

    // TODO: Initialize NetworkClient and connect to server

    // Client-side systems
    m_playerInputSystem = std::make_shared<ecs::PlayerInputSystem>(*m_ecs);
    m_cameraSystem = std::make_shared<ecs::CameraSystem>(*m_ecs, m_aspectRatio);
    m_renderSystem = std::make_shared<ecs::RenderSystem>(*m_ecs, m_meshExecutor, m_cameraSystem, *m_chunkProvider, RENDER_DISTANCE);
    m_uiSystem = std::make_shared<ecs::UISystem>(*m_ecs, windowSize());

    m_ecs->addSystem(m_playerInputSystem);
    m_ecs->addSystem(static_cast<std::shared_ptr<ecs::IRenderableSystem>>(m_cameraSystem));
    m_ecs->addSystem(static_cast<std::shared_ptr<ecs::IRenderableSystem>>(m_renderSystem));
    m_ecs->addSystem(static_cast<std::shared_ptr<ecs::IRenderableSystem>>(m_uiSystem));

    LOG(INFO, "Client initialized");
}

void ClientApplication::shutdown()
{
    m_renderSystem.reset();
    // m_uiSystem.reset();
    m_cameraSystem.reset();
    m_playerInputSystem.reset();
    m_chunkProvider.reset();
    m_ecs.reset();

    LOG(INFO, "Client shutdown");
}

} // namespace mc::core
