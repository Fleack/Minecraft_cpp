#pragma once

#include <FastNoiseLite.h>
#include "world/Chunk.hpp"

namespace mc::world
{
class ChunkGenerator
{
public:
    ChunkGenerator();

    void generate(Chunk& chunk) const;
private:
    FastNoiseLite m_noise{};
};
}
