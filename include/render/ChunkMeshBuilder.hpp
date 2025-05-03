#pragma once

#include <vector>

#include "world/Chunk.hpp"

namespace mc::ecs
{
struct MeshComponent;
}
namespace mc::render
{

/**
 * @brief Generates a mesh for a single chunk using visible block faces.
 */
class ChunkMeshBuilder
{
public:
    /**
     * @brief Builds a mesh from the given chunk.
     *
     * @param chunk Reference to the voxel chunk.
     */
    static std::vector<ecs::MeshComponent> build(world::Chunk const& chunk);
};

} // namespace mc::render
