#pragma once

#include <memory>

namespace mc::render
{
class ChunkMesh;
}

namespace mc::ecs
{
/**
 * @brief Represents a mesh component in the ECS.
 *
 * Holds a reference to a mesh used for rendering a chunk of voxels.
 */
struct MeshComponent
{
    std::shared_ptr<render::ChunkMesh> mesh; ///< Shared pointer to the chunk mesh.
};
} // namespace mc::ecs
