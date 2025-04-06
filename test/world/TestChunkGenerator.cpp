#include <catch2/catch_all.hpp>

#include "world/Block.hpp"
#include "world/Chunk.hpp"
#include "world/ChunkGenerator.hpp"

using namespace mc::world;

TEST_CASE("ChunkGenerator fills chunk with blocks", "[World][ChunkGenerator]")
{
    glm::ivec3 pos{0, 0, 0};
    Chunk chunk(pos);
    ChunkGenerator generator;

    generator.generate(chunk);

    bool hasBlocks = false;

    for (int x = 0; x < CHUNK_SIZE_X && !hasBlocks; ++x)
    {
        for (int z = 0; z < CHUNK_SIZE_Z && !hasBlocks; ++z)
        {
            for (int y = 0; y < CHUNK_SIZE_Y; ++y)
            {
                Block block = chunk.getBlock(x, y, z);
                if (block.type != BlockType::Air)
                {
                    hasBlocks = true;
                    break;
                }
            }
        }
    }

    REQUIRE(hasBlocks);
}

// TODO Remove after ChunkGeneration Improvement
TEST_CASE("ChunkGenerator produces top grass blocks", "[World][ChunkGenerator]")
{
    glm::ivec3 pos{1, 0, 2};
    Chunk chunk(pos);
    ChunkGenerator generator;

    generator.generate(chunk);

    for (int x = 0; x < CHUNK_SIZE_X; ++x)
    {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z)
        {
            int topY = -1;
            for (int y = CHUNK_SIZE_Y - 1; y >= 0; --y)
            {
                if (chunk.getBlock(x, y, z).type != BlockType::Air)
                {
                    topY = y;
                    break;
                }
            }

            if (topY != -1)
            {
                Block topBlock = chunk.getBlock(x, topY, z);
                REQUIRE(topBlock.type == BlockType::Grass);
            }
        }
    }
}

TEST_CASE("ChunkGenerator terrain has some stone underground", "[World][ChunkGenerator]")
{
    glm::ivec3 pos{2, 0, -3};
    Chunk chunk(pos);
    ChunkGenerator generator;

    generator.generate(chunk);

    bool hasStone = false;

    for (int x = 0; x < CHUNK_SIZE_X && !hasStone; ++x)
    {
        for (int z = 0; z < CHUNK_SIZE_Z && !hasStone; ++z)
        {
            for (int y = 0; y < CHUNK_SIZE_Y; ++y)
            {
                if (chunk.getBlock(x, y, z).type == BlockType::Stone)
                {
                    hasStone = true;
                    break;
                }
            }
        }
    }

    REQUIRE(hasStone);
}
