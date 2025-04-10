#pragma once

#include "world/Block.hpp"

#include <string>

namespace mc::render
{
/**
 * @brief Returns the texture name associated with a block type.
 *
 * Maps a BlockType to its corresponding texture name used for rendering.
 *
 * @param type The type of the block.
 * @return Texture name as a string. Returns "error" for unknown types.
 */
inline std::string getTextureNameForBlock(world::BlockType type)
{
    switch (type)
    {
    case world::BlockType::GRASS: return "grass_side";
    case world::BlockType::DIRT: return "dirt";
    case world::BlockType::STONE: return "stone";
    default: return "error";
    }
}
}
