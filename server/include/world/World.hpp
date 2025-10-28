#pragma once

#include "world/ChunkGenerator.hpp"

#include <filesystem>
#include <memory>
#include <optional>
#include <random>
#include <unordered_map>
#include <unordered_set>

#include <Magnum/Math/Vector3.h>
#include <concurrencpp/executors/thread_pool_executor.h>
#include <utils/IVec3Hasher.hpp>

namespace mc::ecs
{
class EventBus;
}

namespace mc::world
{

class Chunk;

/**
 * @brief Manages voxel chunks and procedural generation in the game world.
 *
 * Handles chunk loading, storage, and initial area generation using a
 * procedural terrain generator.
 */
class World
{
public:
    explicit World(
        std::shared_ptr<concurrencpp::thread_pool_executor> chunkExecutor,
        ecs::EventBus& eventBus,
        int32_t seed = std::random_device{}());

    void submitChunkLoad(Magnum::Vector3i const& chunkPos);
    void integrateFinishedChunks();

    [[nodiscard]] Chunk const* getChunk(Magnum::Vector3i const& chunkPos) const;

    [[nodiscard]] bool isChunkLoaded(Magnum::Vector3i const& pos) const;
    [[nodiscard]] bool isChunkPending(Magnum::Vector3i const& pos) const;

    [[nodiscard]] std::unordered_map<Magnum::Vector3i, Chunk, utils::IVec3Hasher> const& getChunks() const;
    [[nodiscard]] std::unordered_set<Magnum::Vector3i, utils::IVec3Hasher> const& getPendingChunks() const;

    size_t unloadChunksOutsideRadius(Magnum::Vector3i const& centerChunk, uint8_t radius);
    [[nodiscard]] size_t getLoadedChunkCount() const;

    void markChunkDirty(Magnum::Vector3i const& chunkPos);

    int32_t getSeed() const;

private:
    void enqueueChunk(Magnum::Vector3i const& chunkPos);
    void commitChunk(Magnum::Vector3i chunkPos, Chunk chunkPtr);

    /**
     * @brief Finds chunks that should be unloaded based on distance.
     *
     * @param centerChunk Center position
     * @param radius Maximum radius to keep loaded
     * @return Vector of chunk positions to unload
     */
    std::vector<Magnum::Vector3i> findChunksToUnload(Magnum::Vector3i const& centerChunk, uint8_t radius) const;

private:
    std::unordered_map<Magnum::Vector3i, Chunk, utils::IVec3Hasher> m_chunks;
    std::unordered_set<Magnum::Vector3i, utils::IVec3Hasher> m_pendingChunks;
    std::unordered_map<Magnum::Vector3i, concurrencpp::result<Chunk>, utils::IVec3Hasher> m_pendingChunkResults;

    std::shared_ptr<concurrencpp::thread_pool_executor> m_chunkExecutor;
    ecs::EventBus& m_eventBus;
    int32_t m_seed;
    ChunkGenerator m_generator;

    // Chunks that have been modified and should be saved before unloading
    // TODO: Implement saving when block modification is added
    std::unordered_set<Magnum::Vector3i, utils::IVec3Hasher> m_dirtyChunks;

    // Path for saving world data to disk
    // TODO: Make configurable, currently using default
    std::filesystem::path m_worldSavePath{"worlds/default"};
};

} // namespace mc::world
