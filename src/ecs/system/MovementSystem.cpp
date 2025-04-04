#include "ecs/system/MovementSystem.hpp"

#include <ranges>

#include "ecs/ECS.hpp"
#include "ecs/component/TransformComponent.hpp"

namespace mc::ecs
{
MovementSystem::MovementSystem(ECS& ecs) : m_ecs(ecs)
{
}

void MovementSystem::Update(float deltaTime) override
{
    auto& transforms = m_ecs.GetAllComponents<TransformComponent>();
    for (auto& transform : transforms | std::views::values)
    {
        transform.x += 0.25f * deltaTime;
    }
}
};
