#include "ecs/system/MovementSystem.hpp"

#include "core/Logger.hpp"
#include "ecs/Ecs.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/component/VelocityComponent.hpp"

#include <ranges>

namespace mc::ecs
{
MovementSystem::MovementSystem(Ecs& ecs)
    : ISystem{Type::MOVEMENT}, m_ecs(ecs)
{
    LOG(INFO, "MovementSystem initialized");
}

void MovementSystem::update(float deltaTime)
{
    for (auto& [entity, velocity] : m_ecs.getAllComponents<VelocityComponent>())
    {
        if (velocity.velocity.isZero()) continue;
        auto* transform = m_ecs.getComponent<TransformComponent>(entity);
        if (!transform)
        {
            LOG(CRITICAL, "No TransformComponent found for entity: {}", entity);
            continue;
        }
        transform->position += Magnum::Vector3d(velocity.velocity * deltaTime);
        if (transform->position.y() < -100.0f)
        {
            LOG(WARN, "Entity {} fell out of the world", entity);
            transform->position.y() = 100.0f;
        }
    }
}
}; // namespace mc::ecs
