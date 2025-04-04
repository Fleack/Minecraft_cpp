#pragma once

#include <memory>

namespace mc::ecs
{
class CameraSystem;
class ECS;
class ISystem;
}

namespace mc::core
{
class Window;

class Application
{
public:
    Application();
    ~Application();

    void run();

private:
    bool initialize();
    void update(float deltaTime);
    void render();
    void shutdown();

    std::unique_ptr<core::Window> m_window;
    std::unique_ptr<ecs::ECS> m_ecs;
    std::shared_ptr<ecs::CameraSystem> m_cameraSystem;

    double m_lastFrameTime = 0.0;
};
}
