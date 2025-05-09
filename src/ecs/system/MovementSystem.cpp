#include "ecs/system/MovementSystem.hpp"

#include "core/Logger.hpp"
#include "ecs/Ecs.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/component/VelocityComponent.hpp"

#include <ranges>

namespace mc::ecs
{
MovementSystem::MovementSystem(Ecs& ecs)
    : m_ecs(ecs)
{
    LOG(INFO, "MovementSystem initialized");
}

void MovementSystem::update(float deltaTime)
{
    for (auto& [entity, velocity] : m_ecs.getAllComponents<VelocityComponent>())
    {
        auto transform = m_ecs.getComponent<TransformComponent>(entity);
        if (!transform)
        {
            LOG(CRITICAL, "No TransformComponent found for entity: {}", entity);
            continue;
        }
        transform->position += Magnum::Vector3d(velocity.velocity * deltaTime);
    }
}
}; // namespace mc::ecs
