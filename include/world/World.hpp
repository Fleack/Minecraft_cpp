#pragma once

#include "utils/IVec3Hasher.hpp"
#include "world/ChunkGenerator.hpp"

#include <memory>
#include <optional>
#include <random>
#include <unordered_map>
#include <unordered_set>

#include <Magnum/Math/Vector3.h>

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
    explicit World(std::shared_ptr<concurrencpp::thread_pool_executor> chunkExecutor, int32_t seed = std::random_device{}());

    void submitChunkLoad(Magnum::Vector3i const& chunkPos);
    void integrateFinishedChunks();

    /**
     * @brief Retrieves a loaded chunk by its position.
     *
     * @param chunkPos Position of the chunk in chunk-space.
     * @return Optional reference to the chunk if loaded.
     */
    [[nodiscard]] std::optional<std::reference_wrapper<Chunk>> getChunk(Magnum::Vector3i const& chunkPos) const;

    /**
     * @brief Checks whether a chunk is currently loaded.
     *
     * @param pos Chunk position to check.
     * @return True if the chunk is present in memory.
     */
    [[nodiscard]] bool isChunkLoaded(Magnum::Vector3i const& pos) const;

    /**
     * @brief Checks whether a chunk is currently pending to be loaded.
     *
     * @param pos Chunk position to check.
     * @return True if the chunk is a pending generation.
     */
    [[nodiscard]] bool isChunkPending(Magnum::Vector3i const& pos) const;

    [[nodiscard]] std::unordered_map<Magnum::Vector3i, std::unique_ptr<Chunk>, utils::IVec3Hasher> const& getChunks() const;
    [[nodiscard]] std::unordered_set<Magnum::Vector3i, utils::IVec3Hasher> const& getPendingChunks() const;

    int32_t getSeed() const;

    static Magnum::Vector3i getChunkOfPosition(Magnum::Vector3i const& position);
    static Magnum::Vector3i getChunkOfPosition(Magnum::Vector3d const& position);

private:
    void enqueueChunk(Magnum::Vector3i const& chunkPos);
    void commitChunk(Magnum::Vector3i chunkPos, std::unique_ptr<Chunk> chunkPtr);

private:
    std::unordered_map<Magnum::Vector3i, std::unique_ptr<Chunk>, utils::IVec3Hasher> m_chunks;
    std::unordered_set<Magnum::Vector3i, utils::IVec3Hasher> m_pendingChunks;
    std::unordered_map<Magnum::Vector3i, concurrencpp::result<std::unique_ptr<Chunk>>, utils::IVec3Hasher> m_pendingChunkResults;

    std::shared_ptr<concurrencpp::thread_pool_executor> m_chunkExecutor;
    int32_t m_seed;
    ChunkGenerator m_generator;
};

} // namespace mc::world
