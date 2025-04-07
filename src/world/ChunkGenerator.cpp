#include "world/ChunkGenerator.hpp"

#include "core/Logger.hpp"

namespace mc::world
{
ChunkGenerator::ChunkGenerator()
{
    m_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_noise.SetFractalOctaves(5);
    m_noise.SetFractalLacunarity(2.0f);
    m_noise.SetFractalGain(0.5f);
    m_noise.SetFrequency(0.005f);
}

void ChunkGenerator::generate(Chunk& chunk) const
{
    const glm::ivec3 origin = chunk.getPosition() * glm::ivec3(CHUNK_SIZE_X, 1, CHUNK_SIZE_Z);
    for (int x = 0; x < CHUNK_SIZE_X; ++x)
    {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z)
        {
            int wx = origin.x + x;
            int wz = origin.z + z;
            float baseNoise = m_noise.GetNoise(static_cast<float>(wx), static_cast<float>(wz));
            float shaped = std::pow(std::abs(baseNoise), 0.8f) * (baseNoise < 0 ? -1.0f : 1.0f);
            float modNoise = m_noise.GetNoise(wx * 0.5f, wz * 0.5f);
            float modifier = std::clamp(modNoise + 0.5f, 0.0f, 1.0f);

            float heightNoise = shaped * modifier;
            int height = static_cast<int>(heightNoise * 24.0f + 64.0f);

            for (int y = 0; y < CHUNK_SIZE_Y; ++y)
            {
                using enum BlockType;
                BlockType type = Air;

                if (y == height) type = Grass;
                else if (y > height - 4 && y < height) type = Dirt;
                else if (y < height) type = Stone;

                chunk.setBlock(x, y, z, Block{type});
            }
        }
    }
    core::Logger::get()->debug(
        std::format("Generated new chunk at [{}, {}]", chunk.getPosition().x, chunk.getPosition().z));
}
}
