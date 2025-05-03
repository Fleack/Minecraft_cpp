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
 * @param face TODO
 * @return Texture name as a string. Returns "error" for unknown types.
 */
inline std::string get_texture_name_for_block(world::BlockType type, uint8_t face)
{
    switch (type)
    {
    case world::BlockType::GRASS:
        if (face == 2) return "grass_top";
        if (face == 3) return "dirt";
        return "grass_side";
    case world::BlockType::DIRT: return "dirt";
    case world::BlockType::STONE: return "stone";
    default: return "error";
    }
}
} // namespace mc::render
