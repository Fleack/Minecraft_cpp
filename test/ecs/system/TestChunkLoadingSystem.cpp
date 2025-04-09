#include <catch2/catch_all.hpp>

#include "ecs/Ecs.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/system/ChunkLoadingSystem.hpp"
#include "world/World.hpp"

using namespace mc::ecs;
using namespace mc::world;

TEST_CASE("ChunkLoadingSystem loads chunks around camera", "[ECS][System][ChunkLoadingSystem]")
{
    Ecs ecs;
    World world;
    int loadRadius = 1;

    ChunkLoadingSystem system(ecs, world, loadRadius);
    ecs.addSystem(std::make_shared<ChunkLoadingSystem>(system));

    Entity camera = ecs.createEntity();
    ecs.addComponent<TransformComponent>(camera, {{0.0f, 0.0f, 0.0f}});

    REQUIRE_FALSE(world.isChunkLoaded({0, 0, 0}));

    ecs.update(0.0f);

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
    World world;
    int loadRadius = 1;

    ChunkLoadingSystem system(ecs, world, loadRadius);
    ecs.addSystem(std::make_shared<ChunkLoadingSystem>(system));

    Entity camera = ecs.createEntity();
    ecs.addComponent<TransformComponent>(camera, {{0.0f, 0.0f, 0.0f}});

    system.update(0.0f);
    size_t chunkCountAfterFirstUpdate = world.getChunks().size();

    ecs.update(0.0f);

    REQUIRE(world.getChunks().size() == chunkCountAfterFirstUpdate);
}

TEST_CASE("ChunkLoadingSystem loads correct chunks when camera moves to new chunk", "[ChunkLoadingSystem]")
{
    Ecs ecs;
    World world;
    uint8_t loadRadius = 1;

    ChunkLoadingSystem system(ecs, world, loadRadius);
    ecs.addSystem(std::make_shared<ChunkLoadingSystem>(system));

    Entity camera = ecs.createEntity();
    ecs.addComponent<TransformComponent>(camera, {{0.0f, 0.0f, 0.0f}});

    ecs.update(0.0f);

    std::set<glm::ivec3, bool(*)(glm::ivec3 const&, glm::ivec3 const&)> firstChunks(
        [](glm::ivec3 const& a, glm::ivec3 const& b) {
            return std::tie(a.x, a.y, a.z) < std::tie(b.x, b.y, b.z);
        });

    for (const auto& [pos, _] : world.getChunks())
    {
        firstChunks.insert(pos);
    }

    auto& transform = ecs.getAllComponents<TransformComponent>()[camera];
    transform.position.x += static_cast<float>(CHUNK_SIZE_X);

    system.update(0.0f);

    std::set<glm::ivec3, bool(*)(glm::ivec3 const&, glm::ivec3 const&)> secondChunks(
        [](glm::ivec3 const& a, glm::ivec3 const& b) {
            return std::tie(a.x, a.y, a.z) < std::tie(b.x, b.y, b.z);
        });

    for (const auto& [pos, _] : world.getChunks())
    {
        secondChunks.insert(pos);
    }

    glm::ivec3 newChunkCenter = {1, 0, 0};

    for (int x = -loadRadius; x <= loadRadius; ++x)
    {
        for (int z = -loadRadius; z <= loadRadius; ++z)
        {
            glm::ivec3 expectedChunk = newChunkCenter + glm::ivec3{x, 0, z};
            REQUIRE(secondChunks.contains(expectedChunk));
        }
    }

    std::vector<glm::ivec3> newLoadedChunks;
    for (const auto& pos : secondChunks)
    {
        if (!firstChunks.contains(pos))
        {
            newLoadedChunks.push_back(pos);
        }
    }

    REQUIRE(newLoadedChunks.size() > 0);
}
