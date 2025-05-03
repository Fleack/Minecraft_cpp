#pragma once

#include <Magnum/Math/Vector3.h>

namespace mc::ecs
{
/**
 * @struct TransformComponent
 * @brief Represents the world position of an entity.
 */
struct TransformComponent
{
    Magnum::Math::Vector3<float> position{0.0f}; ///< World-space position of the entity.
};
} // namespace mc::ecs
