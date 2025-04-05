#include "world/Chunk.hpp"

namespace mc::world
{
Chunk::Chunk(glm::ivec3 position)
    : m_position(position)
{
    m_blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);
}

const glm::ivec3& Chunk::getPosition() const
{
    return m_position;
}

int Chunk::index(int x, int y, int z) const
{
    return x + CHUNK_SIZE_X * (z + CHUNK_SIZE_Z * y);
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
