#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/Window.hpp"

#include <memory>

namespace mc::core
{
static std::unique_ptr<Window> s_Window;

Application::Application()
{
    Logger::Init();
    Logger::GetLogger()->info("Application created");
}

Application::~Application()
{
    Shutdown();
}

bool Application::Initialize()
{
    if (!s_Window)
    {
        s_Window = std::make_unique<Window>("Minecraft", 1280, 720);
    }
    return s_Window->IsOpen();
}

void Application::Run()
{
    if (!Initialize()) return;

    while (s_Window->IsOpen())
    {
        s_Window->PollEvents();

        Update();
        Render();

        s_Window->SwapBuffers();
    }
}

void Application::Update()
{
    // TODO: Input, ECS update, Game logic
}

void Application::Render()
{
    glClearColor(0.15f, 0.15f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Application::Shutdown()
{
    Logger::GetLogger()->info("Shutting down");
    s_Window.reset();
}
}
