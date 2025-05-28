#include "ecs/system/GravitySystem.hpp"

#include "ecs/component/PlayerComponent.hpp"
#include "ecs/component/VelocityComponent.hpp"

namespace mc::ecs
{

GravitySystem::GravitySystem(Ecs& ecs)
    : ISystem{Type::GRAVITY}, m_ecs(ecs) {}

void GravitySystem::update(float dt)
{
    for (auto& [entity, player] : m_ecs.getAllComponents<PlayerComponent>())
    {
        if (!player.onGround)
        {
            if (auto velocity = m_ecs.getComponent<VelocityComponent>(entity))
            {
                auto& velocityY = velocity->velocity.y();
                if (velocityY > -25.0f)
                {
                    velocityY -= gravity * dt;
                }
            }
        }
    }
}

} // namespace mc::ecs
