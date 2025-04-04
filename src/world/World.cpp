#include "world/World.hpp"

namespace mc::world
{
World::World()
{
}

void World::loadChunk(glm::ivec3 const& chunkPos)
{
    if (m_chunks.contains(chunkPos)) { return; }

    auto chunk = std::make_unique<Chunk>(chunkPos);
    m_generator.generate(*chunk);
    m_chunks[chunkPos] = std::move(chunk);
}

std::optional<Chunk> World::getChunk(glm::ivec3 const& chunkPos)
{
    auto it = m_chunks.find(chunkPos);
    if (it != m_chunks.end())
    {
        return *it->second;
    }
    return std::nullopt;
}

void World::generateInitialArea(int radius)
{
    for (int x = -radius; x <= radius; ++x)
    {
        for (int z = -radius; z <= radius; ++z)
        {
            loadChunk({x, 0, z});
        }
    }
}
}
