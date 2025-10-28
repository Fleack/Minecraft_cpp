#pragma once

#include <FastNoiseLite.h>
#include <memory>

#include <world/Chunk.hpp>

namespace mc::world
{
class ChunkGenerator
{
public:
    explicit ChunkGenerator(int32_t seed);

    Chunk generate(Magnum::Vector3i const& chunkPos) const;

private:
    FastNoiseLite m_noise; ///< Noise generator used for terrain shaping.
};
} // namespace mc::world
