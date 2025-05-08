#pragma once

#include "world/Block.hpp"

#include <string>
#include <unordered_map>

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

using texture_id = uint16_t;
inline texture_id get_texture_id_by_name(std::string const& name)
{
    static std::unordered_map<std::string, texture_id> const textureMap = {
        {"grass_top", 1},
        {"grass_side", 2},
        {"dirt", 3},
        {"stone", 4},
    };

    auto it = textureMap.find(name);
    if (it != textureMap.end())
        return it->second;

    return 0;
}
} // namespace mc::render
