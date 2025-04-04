#pragma once

#include "Chunk.hpp"
#include "ChunkGenerator.hpp"

#include <memory>
#include <optional>
#include <unordered_map>
#include <glm/glm.hpp>

namespace mc::world
{
class World
{
public:
    World() = default;

    void loadChunk(glm::ivec3 const& chunkPos);
    [[nodiscard]] std::optional<Chunk> getChunk(glm::ivec3 const& chunkPos);

    void generateInitialArea(int radius = 1);

private:
    std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>, std::hash<glm::ivec3>> m_chunks;
    ChunkGenerator m_generator;
};
}
