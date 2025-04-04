#pragma once

#include "world/ChunkGenerator.hpp"

#include <memory>
#include <optional>
#include <unordered_map>
#include <glm/glm.hpp>

namespace mc::world
{
struct IVec3Hasher
{
    std::size_t operator()(glm::ivec3 const& v) const
    {
        std::size_t h1 = std::hash<int>()(v.x);
        std::size_t h2 = std::hash<int>()(v.y);
        std::size_t h3 = std::hash<int>()(v.z);
        return ((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1);
    }
};

class Chunk;

class World
{
public:
    World();

    void loadChunk(glm::ivec3 const& chunkPos);
    [[nodiscard]] std::optional<Chunk> getChunk(const glm::ivec3& chunkPos);

    void generateInitialArea(int radius = 1);

private:
    std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>, IVec3Hasher> m_chunks;
    ChunkGenerator m_generator;
};
}
