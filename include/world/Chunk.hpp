#pragma once

#include "world/Block.hpp"

#include <vector>

#include <glm/glm.hpp>

namespace mc::world
{
constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 256;
constexpr int CHUNK_SIZE_Z = 16;

class Chunk
{
public:
    using block_volume = std::vector<std::vector<std::vector<Block>>>;
    explicit Chunk(glm::ivec3 position);

    [[nodiscard]] glm::ivec3 const& getPosition() const;

    [[nodiscard]] Block getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, Block block);

private:
    glm::ivec3 m_position;
    block_volume m_blocks;
};
}
