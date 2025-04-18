#pragma once

#include <glm/glm.hpp>

namespace mc::ecs
{
/**
 * @struct TransformComponent
 * @brief Represents the world position of an entity.
 */
struct TransformComponent
{
    glm::vec3 position{0.0f}; ///< World-space position of the entity.
};
} // namespace mc::ecs
