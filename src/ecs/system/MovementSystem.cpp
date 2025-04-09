#include "ecs/system/MovementSystem.hpp"

#include <ranges>

#include "ecs/Ecs.hpp"
#include "ecs/component/TransformComponent.hpp"

namespace mc::ecs
{
MovementSystem::MovementSystem(Ecs& ecs) : m_ecs(ecs)
{
}

void MovementSystem::update(float deltaTime)
{
    auto& transforms = m_ecs.getAllComponents<TransformComponent>();
    for (auto& transform : transforms | std::views::values)
    {
        transform.position.x += 0.25f * deltaTime;
    }
}
};
