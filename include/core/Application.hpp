#pragma once

#include <memory>

namespace mc::world
{
class World;
}

namespace mc::ecs
{
class RenderSystem;
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

    std::unique_ptr<Window> m_window;
    std::unique_ptr<ecs::ECS> m_ecs;
    std::shared_ptr<ecs::CameraSystem> m_cameraSystem;
    std::shared_ptr<ecs::RenderSystem> m_renderSystem;
    std::unique_ptr<world::World> m_world;

    double m_lastFrameTime = 0.0;
};
}
