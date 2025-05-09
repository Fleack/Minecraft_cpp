#include "ecs/system/CameraSystem.hpp"

#include "core/Logger.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "ecs/component/PlayerComponent.hpp"
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

CameraSystem::CameraSystem(Ecs& ecs, float aspectRatio)
    : m_ecs(ecs), m_aspectRatio(aspectRatio)
{
    m_cameraObject = std::make_unique<Object3D>(&m_scene);

    m_cameraObject->translate(Vector3{0.0f, 100.0f, 0.0f});
    m_cameraObject->rotateY(Deg{-90.0f});

    m_camera = std::make_unique<Camera3D>(*m_cameraObject);
    m_camera->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(
            Matrix4::perspectiveProjection(
                Deg{70.0f}, m_aspectRatio, 0.1f, 1000.0f))
        .setViewport(GL::defaultFramebuffer.viewport().size());

    LOG(INFO, "CameraSystem initialized");
}

void CameraSystem::update(float)
{
    auto const& players = m_ecs.getAllComponents<PlayerComponent>();
    if (players.empty())
    {
        LOG(CRITICAL, "No PlayerComponents found");
        return;
    }

    Entity player = players.begin()->first;
    auto transform = m_ecs.getComponent<TransformComponent>(player);
    if (!transform)
    {
        LOG(CRITICAL, "No TransformComponent found for entity: {}", player);
        return;
    }

    auto playerPos = static_cast<Vector3>(transform->position);
    m_cameraObject->setTransformation(Matrix4::translation(playerPos + Vector3{0.0f, 1.6f, 0.0f}) * m_rotationMatrix);

    m_viewMatrix = m_camera->cameraMatrix();
    m_projectionMatrix = m_camera->projectionMatrix();
}

void CameraSystem::render(float)
{
    m_viewMatrix = m_camera->cameraMatrix();
    m_projectionMatrix = m_camera->projectionMatrix();
}

void CameraSystem::handleMouse(Magnum::Vector2 const& delta)
{
    auto& cams = m_ecs.getAllComponents<CameraComponent>();
    if (cams.empty())
    {
        LOG(CRITICAL, "No CameraComponents found!");
        return;
    }

    auto& cam = cams.begin()->second;
    float xoff = -delta.x() * cam.sensitivity;
    float yoff = -delta.y() * cam.sensitivity;

    cam.yaw += Deg{xoff};
    cam.pitch = Math::clamp(Deg{cam.pitch + Deg{yoff}}, Deg{-89.0f}, Deg{89.0f});

    auto qYaw = Quaternion::rotation(cam.yaw, Vector3::yAxis());
    auto qPitch = Quaternion::rotation(cam.pitch, Vector3::xAxis());
    auto q = qYaw * qPitch;
    m_rotationMatrix = Matrix4::from(q.toMatrix(), {});
}

void CameraSystem::handleScroll(float yOffset)
{
    auto& cams = m_ecs.getAllComponents<CameraComponent>();
    if (cams.empty())
    {
        LOG(CRITICAL, "No TransformComponents found!");
        return;
    }

    auto& cam = cams.begin()->second;
    cam.speed *= std::pow(1.1f, yOffset);
    cam.speed = std::clamp(cam.speed, 1.0f, 1000.0f);
}

void CameraSystem::handleKey(Platform::Sdl2Application::Key key, bool pressed)
{
    if (pressed)
        m_keysPressed.insert(key);
    else
        m_keysPressed.erase(key);
}

Magnum::Matrix4 const& CameraSystem::getViewMatrix() const
{
    return m_viewMatrix;
}

Magnum::Matrix4 const& CameraSystem::getProjectionMatrix() const
{
    return m_projectionMatrix;
}

Magnum::Matrix4 const& CameraSystem::getRotationMatrix() const
{
    return m_rotationMatrix;
}

void CameraSystem::setAspectRatio(float ar)
{
    m_aspectRatio = ar;
}

} // namespace mc::ecs
