#pragma once

#include "world/Block.hpp"

#include <vector>

#include <Magnum/Math/Vector3.h>

namespace mc::world
{

constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 256;
constexpr int CHUNK_SIZE_Z = 16;

/**
 * @brief Represents a chunk of voxel blocks in the world.
 *
 * A chunk is a 3D grid of blocks positioned in world space by chunk coordinates.
 * Provides access to individual blocks for querying and editing.
 */
class Chunk
{
public:
    /**
     * @brief Type alias for the 3D block storage container.
     *
     * Represents a 3D array [x][y][z] of Block elements.
     */
    using block_volume = std::vector<std::vector<std::vector<Block>>>;

    /**
     * @brief Constructs a chunk at the specified world chunk position.
     *
     * @param position Chunk position in chunk-space (not world-space).
     */
    explicit Chunk(Magnum::Math::Vector3<int> const& position);

    /**
     * @brief Returns the chunk's position in chunk-space.
     *
     * @return Reference to the internal chunk position vector.
     */
    [[nodiscard]] Magnum::Math::Vector3<int> const& getPosition() const;

    /**
     * @brief Retrieves a block at the specified local chunk coordinates.
     *
     * @param x Local X-coordinate within the chunk.
     * @param y Local Y-coordinate within the chunk.
     * @param z Local Z-coordinate within the chunk.
     *
     * @return The block at the given position.
     */
    [[nodiscard]] Block getBlock(int x, int y, int z) const;

    /**
     * @brief Sets a block at the specified local chunk coordinates.
     *
     * @param x Local X-coordinate within the chunk.
     * @param y Local Y-coordinate within the chunk.
     * @param z Local Z-coordinate within the chunk.
     * @param block The new block to place at the given position.
     */
    void setBlock(int x, int y, int z, Block block);

private:
    Magnum::Math::Vector3<int> m_position; ///< Chunk position in chunk-space (not world-space).
    block_volume m_blocks; ///< 3D grid of blocks within the chunk.
};

} // namespace mc::world
