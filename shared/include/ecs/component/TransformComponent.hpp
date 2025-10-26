#pragma once

#include <Magnum/Math/Vector3.h>

namespace mc::ecs
{
struct TransformComponent
{
    Magnum::Vector3d position{0.0f}; ///< World-space position of the entity.
};
} // namespace mc::ecs
