#include "ecs/system/CameraSystem.hpp"

#include "core/Logger.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "ecs/component/PlayerComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/events/Events.hpp"

#include <Magnum/Math/Functions.h>
#include <Magnum/Math/Quaternion.h>
#include <Magnum/Math/Vector3.h>

using namespace Magnum;

namespace mc::ecs {

CameraSystem::CameraSystem(Ecs& ecs, float aspectRatio)
    : ISystem{Type::CAMERA}, m_ecs{ecs}, m_aspectRatio{aspectRatio}
{
    m_ecs.eventBus().subscribe<ViewportResized>([this](ViewportResized const& e) {
        setAspectRatio(e.aspectRatio);
    });
    LOG(INFO, "CameraSystem initialized");
}

void CameraSystem::update(float)
{
    bool firstActive = true;
    for (auto& [entity, cam] : m_ecs.getAllComponents<CameraComponent>())
    {
        auto* transform = m_ecs.getComponent<TransformComponent>(entity);
        if (!transform)
        {
            LOG(CRITICAL, "No TransformComponent found for entity: {}", entity);
            continue;
        }

        auto qYaw = Quaternion::rotation(cam.yaw, Vector3::yAxis());
        auto qPitch = Quaternion::rotation(cam.pitch, Vector3::xAxis());
        cam.orientation = qYaw * qPitch;
        cam.rotationMatrix = Matrix4::from(cam.orientation.toMatrix(), {});

        Vector3 position = static_cast<Vector3>(transform->position) + Vector3{0.0f, 0.8f, 0.0f};
        Vector3 forward = cam.orientation.transformVector(Vector3::zAxis(-1.0f));
        Vector3 up = cam.orientation.transformVector(Vector3::yAxis());

        cam.viewMatrix = Matrix4::lookAt(position, position + forward, up);
        cam.projectionMatrix = Matrix4::perspectiveProjection(Deg{cam.fov}, m_aspectRatio, 0.1f, 1000.0f);

        if (cam.active && firstActive)
        {
            m_activeCamera = entity;
            m_viewMatrix = cam.viewMatrix;
            m_projectionMatrix = cam.projectionMatrix;
            m_rotationMatrix = cam.rotationMatrix;
            firstActive = false;
        }
    }
}

Magnum::Matrix4 const& CameraSystem::getViewMatrix() const { return m_viewMatrix; }
Magnum::Matrix4 const& CameraSystem::getProjectionMatrix() const { return m_projectionMatrix; }
Magnum::Matrix4 const& CameraSystem::getRotationMatrix() const { return m_rotationMatrix; }

void CameraSystem::setAspectRatio(float ar) { m_aspectRatio = ar; }

} // namespace mc::ecs
