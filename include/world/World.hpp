#pragma once

#include "utils/IVer3Hasher.hpp"
#include "world/ChunkGenerator.hpp"

#include <memory>
#include <optional>
#include <unordered_map>
#include <glm/glm.hpp>

namespace mc::world
{
class Chunk;

class World
{
public:
    World();

    void loadChunk(glm::ivec3 const& chunkPos);
    [[nodiscard]] std::optional<Chunk> getChunk(const glm::ivec3& chunkPos);
    [[nodiscard]] auto const& getChunks() const { return m_chunks; }
    [[nodiscard]] bool isChunkLoaded(glm::ivec3 const& pos) const;

    void generateInitialArea(int radius = 1);

private:
    std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>, utils::IVec3Hasher> m_chunks;
    ChunkGenerator m_generator;
};
}
