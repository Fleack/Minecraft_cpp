#pragma once

#include "CameraComponent.hpp"
#include "MeshComponent.hpp"
#include "ecs/component/ColliderComponent.hpp"
#include "ecs/component/PlayerComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/component/VelocityComponent.hpp"

#include <tuple>

namespace mc::ecs
{

using AllComponentTypes = std::tuple<
    // Shared components
    TransformComponent,
    VelocityComponent,
    PlayerComponent,
    ColliderComponent,
    // Client-only components
    CameraComponent,
    MeshComponent>;

} // namespace mc::ecs
