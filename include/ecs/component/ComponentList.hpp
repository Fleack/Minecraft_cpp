#pragma once

#include "CameraComponent.hpp"
#include "ColliderComponent.hpp"
#include "MeshComponent.hpp"
#include "PlayerComponent.hpp"
#include "TransformComponent.hpp"
#include "VelocityComponent.hpp"

#include <tuple>

namespace mc::ecs
{

using AllComponentTypes = std::tuple<
    CameraComponent,
    ColliderComponent,
    MeshComponent,
    PlayerComponent,
    TransformComponent,
    VelocityComponent>;

} // namespace mc::ecs
