#include "systems/GravitySystem.hpp"

#include <core/Logger.hpp>
#include <ecs/component/PlayerComponent.hpp>
#include <ecs/component/VelocityComponent.hpp>

namespace mc::ecs
{

GravitySystem::GravitySystem(Ecs& ecs)
    : m_ecs(ecs)
{
    LOG(INFO, "GravitySystem initialized");
}

void GravitySystem::update(float dt)
{
    for (auto& [entity, player] : m_ecs.getAllComponents<PlayerComponent>())
    {
        if (!player.onGround)
        {
            if (auto velocity = m_ecs.getComponent<VelocityComponent>(entity))
            {
                auto& velocityY = velocity->velocity.y();
                velocityY = std::max(velocityY - GRAVITY * dt * 2.5, static_cast<double>(TERMINAL_VELOCITY));
            }
        }
    }
}

} // namespace mc::ecs
