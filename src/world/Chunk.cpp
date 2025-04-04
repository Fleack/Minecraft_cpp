#include "world/Chunk.hpp"

namespace mc::world
{
Chunk::Chunk(glm::ivec3 position)
    : m_position(position)
{
}

const glm::ivec3& Chunk::getPosition() const
{
    return m_position;
}

int Chunk::index(int x, int y, int z) const
{
    return x + ChunkSizeX * (z + ChunkSizeZ * y);
}

Block Chunk::getBlock(int x, int y, int z) const
{
    return m_blocks[index(x, y, z)];
}

void Chunk::setBlock(int x, int y, int z, Block block)
{
    m_blocks[index(x, y, z)] = block;
}
}
