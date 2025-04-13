#include <catch2/catch_all.hpp>

#include <glm/glm.hpp>

#include <utils/wait_until.hpp>

#include "global_vars.hpp"
#include "world/Block.hpp"
#include "world/Chunk.hpp"
#include "world/World.hpp"

using namespace mc::world;

TEST_CASE("World::loadChunk and isChunkLoaded", "[World]")
{
    World world{g_runtime.background_executor()};
    glm::ivec3 pos{0, 0, 0};

    REQUIRE_FALSE(world.isChunkLoaded(pos));

    world.loadChunk(pos).get();

    REQUIRE(world.isChunkLoaded(pos));
}

TEST_CASE("World::getChunk returns correct chunk", "[World]")
{
    World world{g_runtime.background_executor()};
    glm::ivec3 pos{1, 0, -2};

    REQUIRE_FALSE(world.getChunk(pos).has_value());

    world.loadChunk(pos).get();

    auto chunkOpt = world.getChunk(pos);
    REQUIRE(chunkOpt.has_value());
    REQUIRE(chunkOpt->get().getPosition() == pos);
}

TEST_CASE("World::loadChunk doesn't reload existing chunk", "[World]")
{
    World world{g_runtime.background_executor()};
    glm::ivec3 pos{3, 0, 5};

    world.loadChunk(pos).get();

    const Chunk* firstChunkPtr = &world.getChunk(pos).value().get();
    world.loadChunk(pos).get();
    const Chunk* secondChunkPtr = &world.getChunk(pos).value().get();

    REQUIRE(firstChunkPtr == secondChunkPtr);
}
