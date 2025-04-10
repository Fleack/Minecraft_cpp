#pragma once

#include "render/ChunkMesh.hpp"
#include "world/Chunk.hpp"

namespace mc::render
{
class TextureAtlas;

class ChunkMeshBuilder
{
public:
    /**
     * @brief Builds the mesh data for a voxel chunk.
     *
     * Generates a triangle mesh based on visible block faces in the chunk,
     * applying proper face geometry, normals, and texture UVs from the atlas.
     *
     * @param chunk The chunk to build the mesh for.
     * @param mesh The target mesh to upload the generated vertex data to.
     * @param atlas The texture atlas for UV lookups.
     */
    static void build(world::Chunk const& chunk, ChunkMesh& mesh, TextureAtlas const& atlas);
};
}
