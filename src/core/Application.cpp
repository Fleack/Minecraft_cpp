#include <glad/gl.h> // MUST be first!

#include "core/Application.hpp"

#include "core/Logger.hpp"
#include "core/Window.hpp"
#include "ecs/Ecs.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/system/CameraSystem.hpp"
#include "ecs/system/ChunkLoadingSystem.hpp"
#include "ecs/system/RenderSystem.hpp"
#include "input/GLFWInputProvider.hpp"
#include "render/Shader.hpp"
#include "render/TextureAtlas.hpp"
#include "world/World.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace mc::core
{
Application::Application(concurrencpp::runtime_options&& options)
    : m_runtime{std::move(options)},
      m_chunkExecutor{m_runtime.thread_pool_executor()}
{
    LOG(INFO, "Application created");
}

Application::~Application()
{
    shutdown();
}

bool Application::initialize()
{
    LOG(INFO, "Initialization started");
    static constexpr uint8_t renderDistance{5};
    if (!initializeWindow())
    {
        LOG(ERROR, "Failed to initialize window");
        return false;
    }

    initializeInput();
    initializeEcs();
    initializeCamera();
    initializeWorld(renderDistance);
    initializeRenderSystems(renderDistance);

    m_lastFrameTime = glfwGetTime();
    LOG(INFO, "Initialization completed");
    return true;
}

bool Application::initializeWindow()
{
    m_window = std::make_unique<Window>("MinecraftX", 1920, 1080);
    LOG(INFO, "Window initialized");
    return m_window->isOpen();
}

void Application::initializeEcs()
{
    m_ecs = std::make_unique<ecs::Ecs>();
    LOG(INFO, "Entity component system initialized");
}

void Application::initializeInput()
{
    m_inputProvider = std::make_shared<input::GlfwInputProvider>(*m_window->getNativeWindow());
    LOG(INFO, "Input provider initialized with native GLFW window");
}

void Application::initializeCamera()
{
    const auto cameraEntity = m_ecs->createEntity();
    m_ecs->addComponent<ecs::CameraComponent>(cameraEntity, ecs::CameraComponent{});
    m_ecs->addComponent<ecs::TransformComponent>(cameraEntity, ecs::TransformComponent{});

    m_cameraSystem = std::make_shared<ecs::CameraSystem>(*m_ecs, 1920.0f / 1080.0f, m_inputProvider);
    m_ecs->addSystem(m_cameraSystem);
    LOG(INFO, "Camera system initialized");
}

void Application::initializeWorld(uint8_t renderDistance)
{
    m_world = std::make_unique<world::World>(m_chunkExecutor);

    m_chunkLoadingSystem = std::make_shared<ecs::ChunkLoadingSystem>(*m_ecs, *m_world, renderDistance);
    m_ecs->addSystem(m_chunkLoadingSystem);
    LOG(INFO, "World initialized with render distance: {}", renderDistance);
}

void Application::initializeRenderSystems(uint8_t renderDistance)
{
    auto atlas = std::make_unique<render::TextureAtlas>(32);
    auto shader = std::make_unique<render::Shader>("shaders/voxel.vert", "shaders/voxel.frag");
    m_renderSystem = std::make_shared<ecs::RenderSystem>(*m_ecs, m_cameraSystem, std::move(shader), std::move(atlas), *m_world, renderDistance);
    m_ecs->addSystem(m_renderSystem);
    LOG(INFO, "Render systems initialized");
}

void Application::run()
{
    LOG(INFO, "Application run loop started");
    if (!initialize())
    {
        LOG(ERROR, "Application failed to initialize");
        return;
    }

    while (m_window->isOpen())
    {
        const double currentTime = glfwGetTime();
        const float deltaTime = static_cast<float>(currentTime - m_lastFrameTime);
        m_lastFrameTime = currentTime;

        m_window->pollEvents();
        update(deltaTime);
        render();
        m_window->swapBuffers();
    }
    LOG(INFO, "Exiting run loop");
}

void Application::update(float deltaTime)
{
    m_ecs->update(deltaTime);
}

void Application::render()
{
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_ecs->render();
}

void Application::shutdown()
{
    LOG(INFO, "Shutting down application");
    m_renderSystem.reset();
    m_cameraSystem.reset();
    m_world.reset();
    m_ecs.reset();
    m_window.reset();
    LOG(INFO, "Shutdown complete");
}
} // namespace mc::core
