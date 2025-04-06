#pragma once

#include "world/Block.hpp"

#include <string>

namespace mc::render
{
inline std::string getTextureNameForBlock(world::BlockType type)
{
    switch (type)
    {
    case world::BlockType::Grass: return "grass_side";
    case world::BlockType::Dirt: return "dirt";
    case world::BlockType::Stone: return "stone";
    default: return "error";
    }
}
}
