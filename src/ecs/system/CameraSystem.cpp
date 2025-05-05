#include "ecs/system/CameraSystem.hpp"

#include "core/Logger.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "ecs/component/TransformComponent.hpp"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Math/Functions.h>
#include <Magnum/Math/Quaternion.h>
#include <Magnum/Math/Vector3.h>

using namespace Magnum;
using namespace Magnum::SceneGraph;
using namespace Magnum::Math::Literals;

namespace mc::ecs
{

CameraSystem::CameraSystem(Ecs& ecs, float aspectRatio, uint8_t renderDistance)
    : m_ecs(ecs), m_aspectRatio(aspectRatio)
{
    m_cameraObject = std::make_unique<Object3D>(&m_scene);

    m_cameraObject->translate(Vector3{0.0f, 100.0f, 0.0f});
    m_cameraObject->rotateY(Deg{-90.0f});

    constexpr float chunkSize = 16.0f;
    float renderDistanceInWorldUnits = renderDistance * chunkSize;

    m_camera = std::make_unique<Camera3D>(*m_cameraObject);
    m_camera->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(
            Matrix4::perspectiveProjection(
                Deg{70.0f}, m_aspectRatio, 0.1f, renderDistanceInWorldUnits))
        .setViewport(GL::defaultFramebuffer.viewport().size());

    auto e = m_ecs.createEntity();
    m_ecs.addComponent<CameraComponent>(e, CameraComponent{});
    m_ecs.addComponent<TransformComponent>(e, TransformComponent{});

    LOG(INFO, "CameraSystem initialized");
}

void CameraSystem::update(float dt)
{
    auto& cams = m_ecs.getAllComponents<CameraComponent>();
    if (cams.empty()) return;
    auto& cam = cams.begin()->second;

    float velocity = cam.speed * dt;

    if (m_keysPressed.contains(Platform::Sdl2Application::Key::W)) m_cameraObject->translateLocal({0.0f, 0.0f, -velocity});
    if (m_keysPressed.contains(Platform::Sdl2Application::Key::S)) m_cameraObject->translateLocal({0.0f, 0.0f, +velocity});
    if (m_keysPressed.contains(Platform::Sdl2Application::Key::A)) m_cameraObject->translateLocal({-velocity, 0.0f, 0.0f});
    if (m_keysPressed.contains(Platform::Sdl2Application::Key::D)) m_cameraObject->translateLocal({+velocity, 0.0f, 0.0f});
    if (m_keysPressed.contains(Platform::Sdl2Application::Key::Space)) m_cameraObject->translate({0.0f, +velocity, 0.0f});
    if (m_keysPressed.contains(Platform::Sdl2Application::Key::LeftCtrl)) m_cameraObject->translate({0.0f, -velocity, 0.0f});

    m_ecs.getAllComponents<TransformComponent>().begin()->second.position = m_cameraObject->transformation().translation();
}

void CameraSystem::render(float)
{
    m_view = m_camera->cameraMatrix();
    m_proj = m_camera->projectionMatrix();
}

void CameraSystem::handleMouse(Math::Vector2<float> const& delta)
{
    auto& cam = m_ecs.getAllComponents<CameraComponent>().begin()->second;

    float xoff = -delta.x() * cam.sensitivity;
    float yoff = -delta.y() * cam.sensitivity;

    cam.yaw += Deg{xoff};
    cam.pitch = Math::clamp(Deg{cam.pitch + Deg{yoff}}, Deg{-89.0f}, Deg{89.0f});

    auto qYaw = Magnum::Math::Quaternion<Float>::rotation(cam.yaw, Vector3::yAxis());
    auto qPitch = Magnum::Math::Quaternion<Float>::rotation(cam.pitch, Vector3::xAxis());
    auto q = qYaw * qPitch;

    Vector3 pos = m_cameraObject->transformation().translation();

    auto rot3 = q.toMatrix();
    Matrix4 matrix = Matrix4::from(rot3, pos);
    m_cameraObject->setTransformation(matrix);
}

void CameraSystem::handleScroll(float yOffset)
{
    auto& cam = m_ecs.getAllComponents<CameraComponent>().begin()->second;
    cam.speed = Math::clamp(cam.speed + yOffset * 5, 1.0f, 100.0f);
}

void CameraSystem::handleKey(Platform::Sdl2Application::Key key, bool pressed)
{
    if (pressed)
        m_keysPressed.insert(key);
    else
        m_keysPressed.erase(key);
}

Magnum::Math::Matrix4<float> const& CameraSystem::getViewMatrix() const
{
    return m_view;
}
Magnum::Math::Matrix4<float> const& CameraSystem::getProjectionMatrix() const
{
    return m_proj;
}

void CameraSystem::setAspectRatio(float ar)
{
    m_aspectRatio = ar;
}

} // namespace mc::ecs
