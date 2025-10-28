#pragma once

#include <cstdint>

namespace mc::world
{

enum class BlockType : uint16_t
{
    AIR = 0, ///< Empty space; non-solid and transparent.
    GRASS, ///< Grass-covered block; solid and opaque.
    DIRT, ///< Dirt block; solid and opaque.
    STONE, ///< Stone block; solid and opaque.
    WATER, ///< Water block; transparent but not air.
};

struct Block
{
    BlockType type{BlockType::AIR};

    [[nodiscard]] bool isTransparent() const
    {
        return type == BlockType::AIR || type == BlockType::WATER;
    }

    [[nodiscard]] bool isSolid() const
    {
        return type != BlockType::AIR;
    }
};
} // namespace mc::world
