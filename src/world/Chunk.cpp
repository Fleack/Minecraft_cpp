#include "world/Chunk.hpp"

namespace mc::world
{
Chunk::Chunk(glm::ivec3 position)
    : m_position(position)
{
    m_blocks.resize(CHUNK_SIZE_X);
    for (auto& column : m_blocks)
    {
        column.resize(CHUNK_SIZE_Y);
        for (auto& row : column)
        {
            row.resize(CHUNK_SIZE_Z);
        }
    }
}

const glm::ivec3& Chunk::getPosition() const
{
    return m_position;
}

Block Chunk::getBlock(int x, int y, int z) const
{
    return m_blocks.at(x).at(y).at(z);
}

void Chunk::setBlock(int x, int y, int z, Block block)
{
    m_blocks.at(x).at(y).at(z) = block;
}
}
