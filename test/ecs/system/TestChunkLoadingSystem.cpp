#include <ranges>

#include <catch2/catch_all.hpp>

#include "global_vars.hpp"

#include "core/Logger.hpp"

#include "ecs/Ecs.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/system/ChunkLoadingSystem.hpp"
#include "utils/wait_until.hpp"
#include "world/World.hpp"

using namespace mc::ecs;
using namespace mc::world;

TEST_CASE("ChunkLoadingSystem loads chunks around camera", "[ECS][System][ChunkLoadingSystem]")
{
    Ecs ecs;
    World world{g_runtime.background_executor()};
    int loadRadius = 1;

    ChunkLoadingSystem system(ecs, world, loadRadius);
    ecs.addSystem(std::make_shared<ChunkLoadingSystem>(system));

    Entity camera = ecs.createEntity();
    ecs.addComponent<TransformComponent>(camera, {{0.0f, 0.0f, 0.0f}});

    REQUIRE_FALSE(world.isChunkLoaded({0, 0, 0}));

    ecs.update(0.0f);
    wait_until([&world]() -> bool { return world.getPendingChunks().empty(); }).get();

    for (int x = -loadRadius; x <= loadRadius; ++x)
    {
        for (int z = -loadRadius; z <= loadRadius; ++z)
        {
            glm::ivec3 pos{x, 0, z};
            INFO(std::format("Chunk pos = [{}, {}, {}]", pos.x, pos.y, pos.z));
            REQUIRE(world.isChunkLoaded(pos));
        }
    }
}

TEST_CASE("ChunkLoadingSystem does not reload chunks when camera stays in same chunk", "[ECS][System][ChunkLoadingSystem]")
{
    Ecs ecs;
    World world{g_runtime.background_executor()};
    int loadRadius = 1;

    ChunkLoadingSystem system(ecs, world, loadRadius);
    ecs.addSystem(std::make_shared<ChunkLoadingSystem>(system));

    Entity camera = ecs.createEntity();
    ecs.addComponent<TransformComponent>(camera, {{0.0f, 0.0f, 0.0f}});

    system.update(0.0f);
    wait_until([&world]() -> bool { return world.getPendingChunks().empty(); }).get();
    size_t chunkCountAfterFirstUpdate = world.getChunks().size();

    ecs.update(0.0f);
    wait_until([&world]() -> bool { return world.getPendingChunks().empty(); }).get();

    REQUIRE(world.getChunks().size() == chunkCountAfterFirstUpdate);
}

TEST_CASE("ChunkLoadingSystem loads correct chunks when camera moves to new chunk", "[ChunkLoadingSystem]")
{
    auto ivec3Comparator = [](const glm::ivec3& a, const glm::ivec3& b) {
        return std::tie(a.x, a.y, a.z) < std::tie(b.x, b.y, b.z);
    };
    using ordered_ivec3_set = std::set<glm::ivec3, decltype(ivec3Comparator)>;

    Ecs ecs;
    World world{g_runtime.background_executor()};
    uint8_t loadRadius = 1;

    ChunkLoadingSystem system(ecs, world, loadRadius);
    ecs.addSystem(std::make_shared<ChunkLoadingSystem>(system));

    Entity camera = ecs.createEntity();
    ecs.addComponent<TransformComponent>(camera, {{0.0f, 0.0f, 0.0f}});

    // First ECS update
    ecs.update(0.0f);
    bool chunksUpdated = wait_until([&world]() -> bool { return world.getPendingChunks().empty(); }).get();
    REQUIRE(chunksUpdated);

    // Saving generated chunks on first ecs update
    ordered_ivec3_set firstChunks(ivec3Comparator);
    for (const auto& pos : world.getChunks() | std::ranges::views::keys)
    {
        firstChunks.insert(pos);
    }

    // Move camera to the next chunk
    auto& transform = ecs.getAllComponents<TransformComponent>()[camera];
    transform.position.x += static_cast<float>(CHUNK_SIZE_X);

    // Second ECS update
    system.update(0.0f);
    chunksUpdated = wait_until([&world]() -> bool { return world.getPendingChunks().empty(); }).get();
    REQUIRE(chunksUpdated);

    // Saving generated chunks on second ecs update
    ordered_ivec3_set secondChunks(ivec3Comparator);
    for (const auto& pos : world.getChunks() | std::ranges::views::keys)
    {
        secondChunks.insert(pos);
    }

    // Check chunks after second ECS update
    glm::ivec3 newChunkCenter = {1, 0, 0};
    for (int x = -loadRadius; x <= loadRadius; ++x)
    {
        for (int z = -loadRadius; z <= loadRadius; ++z)
        {
            glm::ivec3 expectedChunk = newChunkCenter + glm::ivec3{x, 0, z};
            LOG(DEBUG, "Expecting chunk: ({}, {})", expectedChunk.x, expectedChunk.z);
            REQUIRE(secondChunks.contains(expectedChunk));
        }
    }

    // Check that at least one chunk was loaded
    std::vector<glm::ivec3> newLoadedChunks;
    for (const auto& pos : secondChunks)
    {
        if (!firstChunks.contains(pos))
        {
            newLoadedChunks.push_back(pos);
            break;
        }
    }

    LOG(DEBUG, "Newly loaded chunks after camera move: {}", newLoadedChunks.size());
    REQUIRE(newLoadedChunks.size() > 0);
}
