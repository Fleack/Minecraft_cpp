#include "core/Application.hpp"

#include "core/CrashReporter.hpp"
#include "core/Logger.hpp"
#include "ecs/Ecs.hpp"
#include "ecs/component/CameraComponent.hpp"
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
#include <Magnum/Math/Quaternion.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/SceneGraph.h>
#include <Magnum/Trade/MeshData.h>

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

    constexpr uint8_t renderDistance = 2;

    initializeEcs();
    initializeCamera();
    initializeWorld(renderDistance);
    initializeRenderSystems(renderDistance);
    initializeTestCube();

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

void Application::initializeRenderSystems(uint8_t)
{
    // m_renderSystem = std::make_shared<ecs::RenderSystem>(*m_ecs, m_cameraSystem, *m_world, renderDistance);
    // m_ecs->addSystem(m_renderSystem);
}

void Application::initializeTestCube()
{
    using namespace Magnum;

    struct Vertex
    {
        Vector3 position;
        Vector3 normal;
    };

    std::vector<Vertex> vertices = {
        // Front face (+Z)
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},

        // Back face (-Z)
        {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},

        // Top face (+Y)
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},

        // Bottom face (-Y)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},

        // Right face (+X)
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},

        // Left face (-X)
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
    };

    std::vector<UnsignedInt> indices = {
        0, 1, 2, 0, 2, 3, // Front
        4,
        5,
        6,
        4,
        6,
        7, // Back
        8,
        9,
        10,
        8,
        10,
        11, // Top
        12,
        13,
        14,
        12,
        14,
        15, // Bottom
        16,
        17,
        18,
        16,
        18,
        19, // Right
        20,
        21,
        22,
        20,
        22,
        23 // Left
    };

    GL::Buffer vertexBuffer, indexBuffer;
    vertexBuffer.setData(Corrade::Containers::arrayView(vertices.data(), vertices.size()));
    indexBuffer.setData(Corrade::Containers::arrayView(indices.data(), indices.size()));

    typedef GL::Attribute<0, Vector3> PositionAttribute;
    typedef GL::Attribute<1, Vector3> NormalAttribute;

    m_testMesh = GL::Mesh{};
    m_testMesh
        .setCount(indices.size())
        .setPrimitive(GL::MeshPrimitive::Triangles)
        .addVertexBuffer(vertexBuffer, 0, PositionAttribute{}, NormalAttribute{})
        .setIndexBuffer(indexBuffer, 0, GL::MeshIndexType::UnsignedInt);

    m_testShader = std::make_unique<mc::render::ShaderProgram>();
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

    m_ecs->update(deltaTime);
    m_ecs->render();

    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

    m_testShader->setModelMatrix(Matrix4::translation(Vector3::zAxis(-3.0f)))
        .setViewMatrix(m_cameraSystem->getViewMatrix())
        .setProjectionMatrix(m_cameraSystem->getProjectionMatrix());
    m_testShader->draw(m_testMesh);

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
    m_cameraSystem->handleKey(event.key(), true);
    event.setAccepted();
}

void Application::keyReleaseEvent(KeyEvent& event)
{
    m_cameraSystem->handleKey(event.key(), false);
    event.setAccepted();
}

void Application::pointerMoveEvent(PointerMoveEvent& event)
{
    m_cameraSystem->handleMouse(event.relativePosition());
    event.setAccepted();
}

void Application::scrollEvent(ScrollEvent& event)
{
    m_cameraSystem->handleScroll(event.offset().y());
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
