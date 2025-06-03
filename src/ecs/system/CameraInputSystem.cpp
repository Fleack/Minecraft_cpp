#include "ecs/system/CameraInputSystem.hpp"

#include "core/Logger.hpp"
#include "ecs/component/CameraComponent.hpp"

#include <Magnum/Math/Functions.h>
#include <Magnum/Math/Quaternion.h>
#include <Magnum/Math/Vector3.h>

using namespace Magnum;

namespace mc::ecs {

CameraInputSystem::CameraInputSystem(Ecs& ecs)
    : ISystem{Type::CAMERA_INPUT}, m_ecs{ecs}
{
    LOG(INFO, "CameraInputSystem initialized");
}

void CameraInputSystem::update(float) {}

void CameraInputSystem::handleMouse(Vector2 const& delta)
{
    for (auto& [entity, cam] : m_ecs.getAllComponents<CameraComponent>())
    {
        if (!cam.active) continue;
        float xoff = -delta.x() * cam.sensitivity;
        float yoff = -delta.y() * cam.sensitivity;

        cam.yaw += Deg{xoff};
        cam.pitch = Math::clamp(Deg{cam.pitch + Deg{yoff}}, Deg{-89.0f}, Deg{89.0f});
    }
}

} // namespace mc::ecs
