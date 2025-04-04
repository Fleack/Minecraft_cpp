#pragma once

#include <cstdint>

namespace mc::world
{
enum class BlockType : uint16_t
{
    Air = 0,
    Grass,
    Dirt,
    Stone,
    Water,
};

struct Block
{
    BlockType type = BlockType::Air;

    [[nodiscard]] bool isTransparent() const
    {
        return type == BlockType::Air || type == BlockType::Water;
    }

    [[nodiscard]] bool isSolid() const
    {
        return type != BlockType::Air;
    }
};
}
