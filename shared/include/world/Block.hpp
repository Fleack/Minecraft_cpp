#pragma once

#include <cstdint>

namespace mc::world
{
/**
 * @brief Enumeration of available block types in the world.
 *
 * Each block type corresponds to a different material or behavior.
 */
enum class BlockType : uint16_t
{
    AIR = 0, ///< Empty space; non-solid and transparent.
    GRASS, ///< Grass-covered block; solid and opaque.
    DIRT, ///< Dirt block; solid and opaque.
    STONE, ///< Stone block; solid and opaque.
    WATER, ///< Water block; transparent but not air.
};

/**
 * @brief Represents a single voxel block in the world.
 *
 * Contains information about the block's type and helper methods to query
 * rendering and physics-related properties.
 */
struct Block
{
    /**
     * @brief Type of the block.
     */
    BlockType type{BlockType::AIR};

    /**
     * @brief Checks if the block is transparent.
     *
     * Transparent blocks do not fully occlude light or other blocks behind them.
     *
     * @return True if the block is transparent (e.g., air or water).
     */
    [[nodiscard]] bool isTransparent() const
    {
        return type == BlockType::AIR || type == BlockType::WATER;
    }

    /**
     * @brief Checks if the block is solid.
     *
     * Solid blocks occupy space and can block movement.
     *
     * @return True if the block is solid (i.e., not air).
     */
    [[nodiscard]] bool isSolid() const
    {
        return type != BlockType::AIR;
    }
};
} // namespace mc::world
