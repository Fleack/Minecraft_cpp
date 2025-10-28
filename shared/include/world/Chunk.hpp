#pragma once

#include "world/Block.hpp"

#include <array>

#include <Magnum/Math/Vector3.h>

namespace mc::world
{

constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 256;
constexpr int CHUNK_SIZE_Z = 16;

class Chunk
{
public:
    using block_volume = std::array<std::array<std::array<Block, CHUNK_SIZE_Z>, CHUNK_SIZE_Y>, CHUNK_SIZE_X>;

    explicit Chunk(Magnum::Vector3i const& position);

    [[nodiscard]] Magnum::Vector3i const& getPosition() const;

    [[nodiscard]] Block getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, Block block);

    static Magnum::Vector3i getChunkOfPosition(Magnum::Vector3i const& position);
    static Magnum::Vector3i getChunkOfPosition(Magnum::Vector3d const& position);

private:
    Magnum::Vector3i m_position; ///< Chunk position in chunk-space (not world-space).
    block_volume m_blocks; ///< 3D grid of blocks within the chunk.
};

} // namespace mc::world
