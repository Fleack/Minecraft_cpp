#pragma once

#include "ColliderComponent.hpp"
#include "PlayerComponent.hpp"
#include "TransformComponent.hpp"
#include "VelocityComponent.hpp"

#include <tuple>

namespace mc::ecs
{

using AllComponentTypes = std::tuple<
    TransformComponent,
    VelocityComponent,
    PlayerComponent,
    ColliderComponent>;

} // namespace mc::ecs
