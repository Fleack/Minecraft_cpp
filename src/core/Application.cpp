#include "core/Application.hpp"

#include "core/CrashReporter.hpp"
#include "core/Logger.hpp"
#include "ecs/Ecs.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/system/CameraSystem.hpp"
#include "ecs/system/ChunkLoadingSystem.hpp"
#include "ecs/system/RenderSystem.hpp"
#include "world/World.hpp"

#include <chrono>

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>

namespace mc::core
{
using namespace Magnum;
using namespace Magnum::Platform;

Application::Application(Arguments const& arguments)
    : Sdl2Application{
          arguments,
          Configuration{}.setTitle("MinecraftX").setSize({1280, 720}),
          GLConfiguration{}.setVersion(GL::Version::GL460)},
      m_chunkExecutor{m_runtime.thread_pool_executor()}, m_mainExecutor{m_runtime.make_manual_executor()}, m_aspectRatio{static_cast<float>(windowSize().x()) / windowSize().y()}
{
    initializeCore();

    constexpr uint8_t renderDistance = 10;

    initializeEcs();
    initializeCamera();
    initializeWorld(renderDistance);
    initializeRenderSystems(renderDistance);

    setCursor(Cursor::Hidden);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::setClearColor(Color4{0.5f, 0.7f, 1.0f, 1.0f});
    setSwapInterval(1);
    redraw();
}

void Application::initializeCore() const
{
    Logger::init();
    CrashReporter::init();
    LOG(INFO, "Core systems initialized");
}

void Application::initializeEcs()
{
    m_ecs = std::make_unique<ecs::Ecs>();
}

void Application::initializeCamera()
{
    m_cameraSystem = std::make_shared<ecs::CameraSystem>(*m_ecs, m_aspectRatio);
    m_ecs->addSystem(m_cameraSystem);
}

void Application::initializeWorld(uint8_t renderDistance)
{
    m_world = std::make_unique<world::World>(m_chunkExecutor, m_mainExecutor);
    m_chunkLoadingSystem = std::make_shared<ecs::ChunkLoadingSystem>(*m_ecs, *m_world, renderDistance);
    m_ecs->addSystem(m_chunkLoadingSystem);
}

void Application::initializeRenderSystems(uint8_t renderDistance)
{
    m_renderSystem = std::make_shared<ecs::RenderSystem>(*m_ecs, m_cameraSystem, *m_world, renderDistance);
    m_ecs->addSystem(m_renderSystem);
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

    m_ecs->update(deltaTime);
    m_ecs->render();

    swapBuffers();
    redraw();
}

void Application::viewportEvent(ViewportEvent& event)
{
    m_aspectRatio = static_cast<float>(event.windowSize().x()) / event.windowSize().y();
    m_cameraSystem->setAspectRatio(m_aspectRatio);
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
}

void Application::keyPressEvent(KeyEvent& event)
{
    if (event.key() == Key::Esc)
    {
        m_paused = !m_paused;
        if (m_paused)
        {
            setCursor(Cursor::Arrow);
            SDL_SetRelativeMouseMode(SDL_FALSE);
        }
        else
        {
            setCursor(Cursor::Hidden);
            SDL_SetRelativeMouseMode(SDL_TRUE);
        }
    }

    if (!m_paused)
    {
        m_cameraSystem->handleKey(event.key(), true);
    }
    event.setAccepted();
}

void Application::keyReleaseEvent(KeyEvent& event)
{
    if (!m_paused)
    {
        m_cameraSystem->handleKey(event.key(), false);
    }
    event.setAccepted();
}

void Application::pointerMoveEvent(PointerMoveEvent& event)
{
    if (!m_paused)
    {
        m_cameraSystem->handleMouse(event.relativePosition());
    }

    event.setAccepted();
}

void Application::scrollEvent(ScrollEvent& event)
{
    if (!m_paused)
    {
        m_cameraSystem->handleScroll(event.offset().y());
    }
    event.setAccepted();
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
