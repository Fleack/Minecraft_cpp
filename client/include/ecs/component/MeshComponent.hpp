#pragma once

#include "render/BlockTextureMapper.hpp"

#include <memory>

#include <Magnum/GL/Mesh.h>

namespace mc::ecs
{
/**
 * @brief Represents a mesh component in the ECS.
 *
 * Holds a reference to a mesh used for rendering a chunk of voxels.
 */
struct MeshComponent
{
    std::shared_ptr<Magnum::GL::Mesh> mesh; ///< Shared pointer to the chunk mesh.
    render::texture_id textureId;
};
} // namespace mc::ecs
