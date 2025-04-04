#pragma once

#include "render/ChunkMesh.hpp"
#include "world/Chunk.hpp"

namespace mc::render
{
class ChunkMeshBuilder
{
public:
    static void build(const world::Chunk& chunk, ChunkMesh& mesh);
};
}
