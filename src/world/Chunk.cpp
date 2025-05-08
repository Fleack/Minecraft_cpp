#include "world/Chunk.hpp"

#include <array>

namespace mc::world
{

Chunk::Chunk(Magnum::Vector3i const& position)
    : m_position(position)
{}

Magnum::Vector3i const& Chunk::getPosition() const
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

} // namespace mc::world
