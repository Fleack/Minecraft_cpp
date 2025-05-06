#pragma once

#include "world/Chunk.hpp"

#include <vector>

namespace mc::world
{
class World;
}

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
    static std::vector<ecs::MeshComponent> build(world::Chunk const& chunk, world::World const& world);
};

} // namespace mc::render
