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

concurrencpp::lazy_result<void> ChunkGenerator::generate(Chunk& chunk, std::shared_ptr<concurrencpp::executor> executor) const
{
    LOG(DEBUG, "Generating chunk [{}, {}] on thread={}", chunk.getPosition().x, chunk.getPosition().z, std::this_thread::get_id());
    co_await concurrencpp::resume_on(executor);
    const glm::ivec3 origin = chunk.getPosition() * glm::ivec3(CHUNK_SIZE_X, 1, CHUNK_SIZE_Z);

    for (int x = 0; x < CHUNK_SIZE_X; ++x)
    {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z)
        {
            int wx = origin.x + x;
            int wz = origin.z + z;

            // Base noise defines general terrain shape
            float baseNoise = m_noise.GetNoise(static_cast<float>(wx), static_cast<float>(wz));

            // Shape it (curve + preserve sign) to get softer terrain profile
            float shaped = std::pow(std::abs(baseNoise), 0.8f) * (baseNoise < 0 ? -1.0f : 1.0f);

            // Secondary modifier to add variability and smooth blending
            float modNoise = m_noise.GetNoise(wx * 0.5f, wz * 0.5f);
            float modifier = std::clamp(modNoise + 0.5f, 0.0f, 1.0f);

            // Final height calculation, scaled and biased
            int height = static_cast<int>(shaped * modifier * 24.0f + 64.0f);

            for (int y = 0; y < CHUNK_SIZE_Y; ++y)
            {
                using enum BlockType;
                BlockType type = AIR;

                if (y == height) type = GRASS;
                else if (y > height - 4 && y < height) type = DIRT;
                else if (y < height) type = STONE;

                chunk.setBlock(x, y, z, Block{type});
            }
        }
    }

    LOG(DEBUG, "Generated new chunk at [{}, {}] on thread={}", chunk.getPosition().x, chunk.getPosition().z, std::this_thread::get_id());
}
} // namespace mc::world
