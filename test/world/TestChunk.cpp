#include "world/Block.hpp"
#include "world/Chunk.hpp"

#include <catch2/catch_all.hpp>
#include <glm/glm.hpp>

using namespace mc::world;

TEST_CASE("Chunk::getPosition returns correct position", "[World][Chunk]")
{
    glm::ivec3 pos{5, 0, -3};
    Chunk chunk(pos);

    REQUIRE(chunk.getPosition() == pos);
}

TEST_CASE("Chunk::setBlock and getBlock work correctly", "[World][Chunk]")
{
    glm::ivec3 pos{0, 0, 0};
    Chunk chunk(pos);

    Block block{BlockType::Grass};

    chunk.setBlock(1, 2, 3, block);
    Block result = chunk.getBlock(1, 2, 3);

    REQUIRE(result.type == BlockType::Grass);
}

TEST_CASE("Chunk throws on out-of-bounds access", "[World][Chunk]")
{
    glm::ivec3 pos{0, 0, 0};
    Chunk chunk(pos);

    int maxX = CHUNK_SIZE_X;
    int maxY = CHUNK_SIZE_Y;
    int maxZ = CHUNK_SIZE_Z;

    REQUIRE_THROWS_AS(chunk.getBlock(maxX, 0, 0), std::out_of_range);
    REQUIRE_THROWS_AS(chunk.getBlock(0, maxY, 0), std::out_of_range);
    REQUIRE_THROWS_AS(chunk.getBlock(0, 0, maxZ), std::out_of_range);

    REQUIRE_THROWS_AS(chunk.setBlock(maxX, 0, 0, {BlockType::Stone}), std::out_of_range);
    REQUIRE_THROWS_AS(chunk.setBlock(0, maxY, 0, {BlockType::Stone}), std::out_of_range);
    REQUIRE_THROWS_AS(chunk.setBlock(0, 0, maxZ, {BlockType::Stone}), std::out_of_range);
}
