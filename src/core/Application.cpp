#include <glad/gl.h> // MUST be first!

#include "core/Application.hpp"

#include "core/Logger.hpp"
#include "core/Window.hpp"
#include "ecs/ECS.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "ecs/component/MeshComponent.hpp"
#include "ecs/system/CameraSystem.hpp"
#include "ecs/system/RenderSystem.hpp"
#include "render/ChunkMesh.hpp"
#include "render/ChunkMeshBuilder.hpp"
#include "world/World.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace mc::core
{
Application::Application()
{
    Logger::init();
    Logger::get()->info("Application created");
}

Application::~Application()
{
    shutdown();
}

bool Application::initialize()
{
    m_window = std::make_unique<Window>("VoxelGame", 1280, 720);
    if (!m_window->isOpen()) { return false; }

    m_ecs = std::make_unique<ecs::ECS>();

    // Camera
    const auto cameraEntity = m_ecs->createEntity();
    m_ecs->addComponent<ecs::CameraComponent>(cameraEntity, ecs::CameraComponent{});
    m_cameraSystem = std::make_shared<ecs::CameraSystem>(*m_ecs, 1280.0f / 720.0f);
    m_ecs->addSystem(m_cameraSystem);

    // World
    m_world = std::make_unique<world::World>();
    m_world->generateInitialArea(1);

    // Meshes
    for (int x = -1; x <= 1; ++x)
    {
        for (int z = -1; z <= 1; ++z)
        {
            const glm::ivec3 pos{x, 0, z};
            auto chunk = m_world->getChunk(pos);
            if (!chunk) continue;

            auto mesh = std::make_shared<render::ChunkMesh>();
            render::ChunkMeshBuilder::build(*chunk, *mesh);

            auto entity = m_ecs->createEntity();
            m_ecs->addComponent<ecs::MeshComponent>(entity, ecs::MeshComponent{mesh});
        }
    }

    // Render system
    m_renderSystem = std::make_shared<ecs::RenderSystem>(*m_ecs, m_cameraSystem);
    m_ecs->addSystem(m_renderSystem);

    m_lastFrameTime = glfwGetTime();
    return true;
}

void Application::run()
{
    if (!initialize()) return;

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
}

void Application::update(float deltaTime)
{
    m_ecs->update(deltaTime);
}

void Application::render()
{
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Application::shutdown()
{
    Logger::get()->info("Shutting down application");
    m_renderSystem.reset();
    m_cameraSystem.reset();
    m_world.reset();
    m_ecs.reset();
    m_window.reset();
}
}
