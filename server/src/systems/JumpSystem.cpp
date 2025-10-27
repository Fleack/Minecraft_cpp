#include "systems/JumpSystem.hpp"

#include <core/Logger.hpp>
#include <ecs/component/PlayerComponent.hpp>
#include <ecs/component/VelocityComponent.hpp>

namespace mc::ecs
{

JumpSystem::JumpSystem(Ecs& ecs)
    : m_ecs(ecs)
{
    LOG(INFO, "JumpSystem initialized");
}

void JumpSystem::update(float)
{
    for (auto& [entity, player] : m_ecs.getAllComponents<PlayerComponent>())
    {
        if (!player.wantsToJump)
            continue;

        player.wantsToJump = false;

        if (!player.onGround)
            continue;

        if (auto velocity = m_ecs.getComponent<VelocityComponent>(entity); velocity->velocity.y() <= 0.01f)
        {
            velocity->velocity.y() = JUMP_FORCE;
        }
    }
}

} // namespace mc::ecs
