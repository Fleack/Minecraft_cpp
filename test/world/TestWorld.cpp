#include <catch2/catch_all.hpp>

#include <glm/glm.hpp>

#include "world/Block.hpp"
#include "world/Chunk.hpp"
#include "world/World.hpp"

using namespace mc::world;

TEST_CASE("World::loadChunk and isChunkLoaded", "[World]")
{
    World world;
    glm::ivec3 pos{0, 0, 0};

    REQUIRE_FALSE(world.isChunkLoaded(pos));

    world.loadChunk(pos);

    REQUIRE(world.isChunkLoaded(pos));
}

TEST_CASE("World::getChunk returns correct chunk", "[World]")
{
    World world;
    glm::ivec3 pos{1, 0, -2};

    REQUIRE_FALSE(world.getChunk(pos).has_value());

    world.loadChunk(pos);

    auto chunkOpt = world.getChunk(pos);
    REQUIRE(chunkOpt.has_value());
    REQUIRE(chunkOpt->get().getPosition() == pos);
}

TEST_CASE("World::loadChunk doesn't reload existing chunk", "[World]")
{
    World world;
    glm::ivec3 pos{3, 0, 5};

    world.loadChunk(pos);

    const Chunk* firstChunkPtr = &world.getChunk(pos).value().get();
    world.loadChunk(pos);
    const Chunk* secondChunkPtr = &world.getChunk(pos).value().get();

    REQUIRE(firstChunkPtr == secondChunkPtr);
}

TEST_CASE("World::generateInitialArea loads correct chunks", "[World]")
{
    World world;

    int radius = 3;
    world.generateInitialArea(radius);

    for (int x = -radius; x <= radius; ++x)
    {
        for (int z = -radius; z <= radius; ++z)
        {
            glm::ivec3 pos{x, 0, z};
            REQUIRE(world.isChunkLoaded(pos));
        }
    }

    glm::ivec3 outsidePos{radius + 1, 0, 0};
    REQUIRE_FALSE(world.isChunkLoaded(outsidePos));
}
