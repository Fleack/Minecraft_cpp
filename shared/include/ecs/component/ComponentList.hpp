#pragma once

#include "TransformComponent.hpp"
#include "VelocityComponent.hpp"
#include "PlayerComponent.hpp"
#include "ColliderComponent.hpp"

#include <tuple>

namespace mc::ecs
{

using AllComponentTypes = std::tuple<
    TransformComponent,
    VelocityComponent,
    PlayerComponent,
    ColliderComponent>;

} // namespace mc::ecs

