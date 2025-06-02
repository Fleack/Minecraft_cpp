#include "ecs/system/PlayerInputSystem.hpp"

#include "core/Logger.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "ecs/component/PlayerComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/component/VelocityComponent.hpp"
#include "ecs/system/CameraSystem.hpp"

#include <ranges>

#include <Magnum/Math/Quaternion.h>

using namespace Magnum;

namespace mc::ecs
{

PlayerInputSystem::PlayerInputSystem(Ecs& ecs)
    : ISystem{Type::PLAYER_INPUT}, m_ecs(ecs) {}

void PlayerInputSystem::update(float dt)
{
    for (auto const& entity : m_ecs.getAllComponents<PlayerComponent>() | std::views::keys)
    {
        auto velocity = m_ecs.getComponent<VelocityComponent>(entity);
        if (!velocity)
        {
            LOG(CRITICAL, "No VelocityComponent found for entity: {}", entity);
            continue;
        }

        Math::Vector3 inputDir{0.0f};
        if (m_keysPressed.contains(Platform::Sdl2Application::Key::W)) inputDir.z() -= 1.0f;
        if (m_keysPressed.contains(Platform::Sdl2Application::Key::S)) inputDir.z() += 1.0f;
        if (m_keysPressed.contains(Platform::Sdl2Application::Key::A)) inputDir.x() -= 1.0f;
        if (m_keysPressed.contains(Platform::Sdl2Application::Key::D)) inputDir.x() += 1.0f;

        if (m_keysPressed.contains(Platform::Sdl2Application::Key::R))
        {
            auto& transform = *m_ecs.getComponent<TransformComponent>(entity);
            transform.position.y() = 100.0f;
        }

        auto& cams = m_ecs.getAllComponents<CameraComponent>();
        if (cams.empty())
        {
            LOG(CRITICAL, "No CameraComponent found!");
            return;
        }
        auto& cam = cams.begin()->second;

        auto qYaw = Math::Quaternion<float>::rotation(cam.yaw, Vector3::yAxis());
        inputDir = qYaw.transformVector(inputDir);

        if (!inputDir.isZero())
            inputDir = inputDir.normalized();

        velocity->desiredVelocity = static_cast<Magnum::Vector3d>(inputDir * velocity->maxSpeed);

        auto springMove = [](float current, float target, float stiffness, float damping, float dt) -> float {
            float delta = target - current;
            float accel = delta * stiffness - current * damping;
            return current + accel * dt;
        };

        auto& v = velocity->velocity;
        v.x() = springMove(v.x(), velocity->desiredVelocity.x(), velocity->acceleration, velocity->damping, dt);
        v.z() = springMove(v.z(), velocity->desiredVelocity.z(), velocity->acceleration, velocity->damping, dt);
    }
}

void PlayerInputSystem::handleKey(Platform::Sdl2Application::Key key, bool pressed)
{
    if (pressed)
        m_keysPressed.insert(key);
    else
        m_keysPressed.erase(key);

    if (pressed && key == Platform::Sdl2Application::Key::Space)
    {
        for (auto& player : m_ecs.getAllComponents<PlayerComponent>() | std::views::values)
        {
            if (player.onGround)
            {
                player.wantsToJump = true;
            }
        }
    }
}

void PlayerInputSystem::handleScroll(float yOffset)
{
    auto& velocityComponents = m_ecs.getAllComponents<VelocityComponent>();
    if (velocityComponents.empty())
    {
        LOG(CRITICAL, "No VelocityComponents found!");
        return;
    }

    auto& velocity = velocityComponents.begin()->second;
    velocity.maxSpeed *= std::pow(1.1f, yOffset);
    velocity.maxSpeed = std::clamp(velocity.maxSpeed, 1.0f, 1000.0f);
}

} // namespace mc::ecs
