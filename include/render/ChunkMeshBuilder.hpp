#pragma once

#include "render/ChunkMesh.hpp"
#include "world/Chunk.hpp"

namespace mc::render
{
class TextureAtlas;

class ChunkMeshBuilder
{
public:
    static void build(world::Chunk const& chunk, ChunkMesh& mesh, TextureAtlas const& atlas);
};
}
