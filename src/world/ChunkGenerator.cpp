#include "world/ChunkGenerator.hpp"

namespace mc::world
{
ChunkGenerator::ChunkGenerator()
{
    m_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_noise.SetFrequency(0.01f);
}

void ChunkGenerator::generate(Chunk& chunk) const
{
    const auto& pos = chunk.getPosition();

    for (int x = 0; x < ChunkSizeX; ++x)
    {
        for (int z = 0; z < ChunkSizeZ; ++z)
        {
            float worldX = static_cast<float>(pos.x * ChunkSizeX + x);
            float worldZ = static_cast<float>(pos.z * ChunkSizeZ + z);

            float height = m_noise.GetNoise(worldX, worldZ);
            int maxY = static_cast<int>((height + 1.0f) * 0.5f * 64.0f) + 32;

            for (int y = 0; y < maxY; ++y)
            {
                BlockType type = BlockType::Dirt;
                if (y == maxY - 1) type = BlockType::Grass;
                else if (y < maxY - 5) type = BlockType::Stone;

                chunk.setBlock(x, y, z, {type});
            }
        }
    }
}
}
