#pragma once

#include "world/Chunk.hpp"

#include <Magnum/GL/Buffer.h>

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
     * @param mesh Output a mesh object.
     * @param uvTileSize Size of a single texture tile in UV space (e.g., 1/atlasSize).
     */
    static void build(world::Chunk const& chunk, Magnum::GL::Mesh& mesh, float uvTileSize);
};

} // namespace mc::render
