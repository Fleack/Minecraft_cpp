#include <glad/gl.h> // Must be first!

#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/Window.hpp"

#include "ecs/ECS.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "ecs/system/CameraSystem.hpp"

#include <GLFW/glfw3.h>

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
    m_window = std::make_unique<Window>("Minecraft", 1280, 720);
    if (!m_window->isOpen())
    {
        return false;
    }

    m_ecs = std::make_unique<ecs::ECS>();

    const auto cameraEntity = m_ecs->createEntity();
    m_ecs->addComponent<ecs::CameraComponent>(cameraEntity, ecs::CameraComponent{});

    m_cameraSystem = std::make_shared<ecs::CameraSystem>(*m_ecs, 1280.0f / 720.0f);
    m_ecs->addSystem(m_cameraSystem);

    m_lastFrameTime = glfwGetTime();
    return true;
}

void Application::run()
{
    if (!initialize()) return;

    while (m_window->isOpen())
    {
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - m_lastFrameTime);
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
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO render calls
}

void Application::shutdown()
{
    Logger::get()->info("Application shutdown");
    m_cameraSystem.reset();
    m_ecs.reset();
    m_window.reset();
}
}
