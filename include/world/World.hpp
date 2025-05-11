#pragma once

#include "utils/IVec3Hasher.hpp"
#include "world/ChunkGenerator.hpp"

#include <memory>
#include <optional>
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
    explicit World(std::shared_ptr<concurrencpp::thread_pool_executor> chunkExecutor);

    /**
     * @brief Asynchronously loads a chunk if not already loaded or pending.
     *
     * Enqueues and generates the chunk asynchronously if it isn't present.
     * @param chunkPos Position of the chunk in chunk-space.
     * @return Lazy result completing when the chunk is ready.
     */
    concurrencpp::lazy_result<void> loadChunk(Magnum::Vector3i chunkPos);

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

    static Magnum::Vector3i getChunkOfPosition(Magnum::Vector3i const& position);
    static Magnum::Vector3i getChunkOfPosition(Magnum::Vector3d const& position);

private:
    void enqueueChunk(Magnum::Vector3i const& chunkPos);
    void commitChunk(Magnum::Vector3i chunkPos, std::unique_ptr<Chunk> chunkPtr);
    concurrencpp::lazy_result<std::unique_ptr<Chunk>> generateChunkAsync(Magnum::Vector3i chunkPos) const;

private:
    std::unordered_map<Magnum::Vector3i, std::unique_ptr<Chunk>, utils::IVec3Hasher> m_chunks;
    std::unordered_set<Magnum::Vector3i, utils::IVec3Hasher> m_pendingChunks;

    std::shared_ptr<concurrencpp::thread_pool_executor> m_chunkExecutor;
    ChunkGenerator m_generator;
};

} // namespace mc::world
