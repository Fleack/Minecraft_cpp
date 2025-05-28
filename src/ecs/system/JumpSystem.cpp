#include "ecs/system/JumpSystem.hpp"

#include "ecs/component/PlayerComponent.hpp"
#include "ecs/component/VelocityComponent.hpp"

namespace mc::ecs
{

JumpSystem::JumpSystem(Ecs& ecs)
    : ISystem{Type::JUMP}, m_ecs(ecs) {}

void JumpSystem::update(float)
{
    for (auto& [entity, player] : m_ecs.getAllComponents<PlayerComponent>())
    {
        if (player.wantsToJump)
        {
            if (player.onGround)
            {
                if (auto velocity = m_ecs.getComponent<VelocityComponent>(entity))
                {
                    static constexpr float jumpForce = 3.5f;
                    velocity->velocity.y() = jumpForce;
                    player.onGround = false;
                }
            }

            player.wantsToJump = false;
        }
    }
}

} // namespace mc::ecs
