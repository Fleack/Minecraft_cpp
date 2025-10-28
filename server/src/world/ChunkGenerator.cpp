#include "world/ChunkGenerator.hpp"

#include <core/Logger.hpp>

#include <algorithm>
#include <cmath>

#include <Magnum/Math/Vector3.h>

namespace mc::world
{

ChunkGenerator::ChunkGenerator(int32_t seed)
    : m_noise{seed}
{
    m_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_noise.SetFractalOctaves(5);
    m_noise.SetFractalLacunarity(2.0f);
    m_noise.SetFractalGain(0.5f);
    m_noise.SetFrequency(0.005f);
}

Chunk ChunkGenerator::generate(Magnum::Vector3i const& chunkPos) const
{
    Chunk chunk{chunkPos};
    Magnum::Vector3i const origin = chunkPos * Magnum::Vector3i{CHUNK_SIZE_X, 0, CHUNK_SIZE_Z};

    for (int x = 0; x < CHUNK_SIZE_X; ++x)
    {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z)
        {
            int const wx = origin.x() + x;
            int const wz = origin.z() + z;

            // Base noise defines a general terrain shape
            float const baseNoise = m_noise.GetNoise(static_cast<float>(wx), static_cast<float>(wz));

            // Shape it (curve + preserve sign) to get a softer terrain profile
            float const shaped = std::pow(std::abs(baseNoise), 0.8f) *
                (baseNoise < 0.0f ? -1.0f : 1.0f);

            // Secondary modifier to add variability and smooth blending
            float const modNoise = m_noise.GetNoise(wx * 0.5f, wz * 0.5f);
            float const modifier = std::clamp(modNoise + 0.5f, 0.0f, 1.0f);

            // Final height calculation, scaled and biased
            int const height = static_cast<int>(shaped * modifier * 24.0f + 64.0f);

            for (int y = 0; y < CHUNK_SIZE_Y; ++y)
            {
                using enum BlockType;
                auto type = AIR;

                if (y == height)
                    type = GRASS;
                else if (y > height - 4 && y < height)
                    type = DIRT;
                else if (y < height)
                    type = STONE;

                chunk.setBlock(x, y, z, Block{type});
            }
        }
    }

    return chunk;
}

} // namespace mc::world
