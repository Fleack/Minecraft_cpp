#include "world/Chunk.hpp"

#include "Magnum/Math/Functions.h"
#include "utils/FastDivFloor.hpp"

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

Magnum::Vector3i Chunk::getChunkOfPosition(Magnum::Vector3i const& position)
{
    return {
        utils::floor_div(position.x(), CHUNK_SIZE_X),
        0,
        utils::floor_div(position.z(), CHUNK_SIZE_Z)};
}

Magnum::Vector3i Chunk::getChunkOfPosition(Magnum::Vector3d const& position)
{
    return getChunkOfPosition(Magnum::Vector3i{Magnum::Math::floor(position)});
}

} // namespace mc::world
