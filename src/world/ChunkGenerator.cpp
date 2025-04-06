#include "world/ChunkGenerator.hpp"

#include "core/Logger.hpp"

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

    for (int x = 0; x < CHUNK_SIZE_X; ++x)
    {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z)
        {
            float worldX = static_cast<float>(pos.x * CHUNK_SIZE_X + x);
            float worldZ = static_cast<float>(pos.z * CHUNK_SIZE_Z + z);

            float height = m_noise.GetNoise(worldX, worldZ);
            int maxY = static_cast<int>((height + 1.0f) * 32.0f) + 32;

            for (int y = 0; y < maxY; ++y)
            {
                auto type = BlockType::Dirt;
                if (y == maxY - 1) type = BlockType::Grass;
                else if (y < maxY - 5) type = BlockType::Stone;

                chunk.setBlock(x, y, z, {type});
            }
        }
    }
    core::Logger::get()->debug(std::format("Generated new chunk at [{}, {}]", static_cast<float>(pos.x), static_cast<float>(pos.z)));
}
}
