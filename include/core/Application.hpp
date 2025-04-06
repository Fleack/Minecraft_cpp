#pragma once

#include <memory>

namespace mc::input
{
class IInputProvider;
}

namespace mc::world
{
class World;
}

namespace mc::ecs
{
class ChunkLoadingSystem;
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

    bool initializeWindow();
    void initializeECS();
    void initializeInput();
    void initializeCamera();
    void initializeWorld();
    void initializeRenderSystems();

    void update(float deltaTime);
    void render();
    void shutdown();

    std::unique_ptr<Window> m_window;
    std::unique_ptr<ecs::ECS> m_ecs;
    std::shared_ptr<input::IInputProvider> m_inputProvider;
    std::shared_ptr<ecs::CameraSystem> m_cameraSystem;
    std::shared_ptr<ecs::RenderSystem> m_renderSystem;
    std::shared_ptr<ecs::ChunkLoadingSystem> m_chunkLoadingSystem;
    std::unique_ptr<world::World> m_world;

    double m_lastFrameTime = 0.0;
};
}
