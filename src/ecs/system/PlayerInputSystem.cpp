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

void PlayerInputSystem::update(float)
{
    for (auto const& entity : m_ecs.getAllComponents<PlayerComponent>() | std::views::keys)
    {
        auto velocity = m_ecs.getComponent<VelocityComponent>(entity);
        if (!velocity)
        {
            LOG(CRITICAL, "No VelocityComponent found for entity: {}", entity);
            continue;
        }

        Vector3 move{0.0f};
        if (m_keysPressed.contains(Platform::Sdl2Application::Key::W)) move.z() -= 1.0f;
        if (m_keysPressed.contains(Platform::Sdl2Application::Key::S)) move.z() += 1.0f;
        if (m_keysPressed.contains(Platform::Sdl2Application::Key::A)) move.x() -= 1.0f;
        if (m_keysPressed.contains(Platform::Sdl2Application::Key::D)) move.x() += 1.0f;

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
        move = qYaw.transformVector(move);

        if (move.isZero())
        {
            velocity->velocity.x() = 0.0f;
            velocity->velocity.z() = 0.0f;
        }
        else
        {
            move = move.normalized() * velocity->speed;
            velocity->velocity.x() = move.x();
            velocity->velocity.z() = move.z();
        }
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
    velocity.speed *= std::pow(1.1f, yOffset);
    velocity.speed = std::clamp(velocity.speed, 1.0f, 1000.0f);
}

} // namespace mc::ecs
