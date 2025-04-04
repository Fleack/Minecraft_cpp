#pragma once

#include "world/Block.hpp"

#include <array>
#include <glm/glm.hpp>

namespace mc::world
{
constexpr int ChunkSizeX = 16;
constexpr int ChunkSizeY = 256;
constexpr int ChunkSizeZ = 16;

class Chunk
{
public:
    explicit Chunk(glm::ivec3 position);

    [[nodiscard]] glm::ivec3 const& getPosition() const;

    [[nodiscard]] Block getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, Block block);

private:
    glm::ivec3 m_position;
    std::array<Block, ChunkSizeX * ChunkSizeY * ChunkSizeZ> m_blocks;

    [[nodiscard]] int index(int x, int y, int z) const;
};
}
