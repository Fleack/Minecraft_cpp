#pragma once

#include "utils/IVer3Hasher.hpp"
#include "world/ChunkGenerator.hpp"

#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include <glm/glm.hpp>

namespace mc::world
{
class Chunk;

/**
 * @brief Manages voxel chunks and procedural generation in the game world.
 *
 * Handles chunk loading, storage, and initial area generation. Uses a procedural
 * terrain generator to populate chunks as needed.
 */
class World
{
public:
    World(
        std::shared_ptr<concurrencpp::thread_pool_executor> chunkExecutor,
        std::shared_ptr<concurrencpp::manual_executor> mainExec);

    /**
     * @brief Asynchronously loads a chunk if not already loaded or pending.
     *
     * If the chunk is neither generated nor being generated, it enqueues it,
     * generates it asynchronously, and commits it into the world.
     *
     * @param chunkPos Position of the chunk to load.
     * @return Lazy result completing when the chunk is ready.
     */
    concurrencpp::lazy_result<void> loadChunk(glm::ivec3 chunkPos);

    /**
     * @brief Retrieves a loaded chunk by its position.
     *
     * @param chunkPos Position of the chunk in chunk-space.
     * @return Optional reference to the chunk if found, otherwise std::nullopt.
     */
    [[nodiscard]] std::optional<std::reference_wrapper<Chunk>> getChunk(glm::ivec3 const& chunkPos);

    /**
     * @brief Checks whether a chunk is currently loaded.
     *
     * @param pos Chunk position to check.
     * @return True if the chunk is present in memory.
     */
    [[nodiscard]] bool isChunkLoaded(glm::ivec3 const& pos) const;

    /**
     * @brief Checks whether a chunk is currently pending to be loaded.
     *
     * @param pos Chunk position to check.
     * @return True if the chunk is pending to be loaded in memory.
     */
    [[nodiscard]] bool isChunkPending(glm::ivec3 const& pos) const;

    [[nodiscard]] auto const& getChunks() const
    {
        return m_chunks;
    }

    [[nodiscard]] auto const& getPendingChunks() const
    {
        return m_pendingChunks;
    }

private:
    /**
     * @brief Queues a chunk for generation.
     *
     * Adds the chunk position to the set of pending chunks to prevent duplicate generation.
     *
     * @param chunkPos Position of the chunk in chunk-space.
     */
    void enqueueChunk(glm::ivec3 chunkPos);

    /**
     * @brief Asynchronously generates a new chunk using the background executor.
     *
     * This coroutine schedules chunk generation work on the chunk thread pool.
     * It creates a new chunk and calls the procedural generator.
     *
     * @param chunkPos Position of the chunk to generate.
     * @return Lazy result resolving to a unique pointer to the generated chunk.
     */
    concurrencpp::lazy_result<std::unique_ptr<Chunk>> generateChunkAsync(glm::ivec3 chunkPos) const;

    /**
     * @brief Asynchronously commits a generated chunk to the world.
     *
     * This coroutine schedules execution back onto the main thread and inserts
     * the generated chunk into the active chunk map.
     *
     * @param chunkPos Position of the chunk.
     * @param chunkPtr Unique pointer to the generated chunk.
     * @return Lazy result completing when the chunk is committed.
     */
    concurrencpp::lazy_result<void> commitChunkAsync(glm::ivec3 chunkPos, std::unique_ptr<Chunk> chunkPtr);

private:
    std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>, utils::IVec3Hasher> m_chunks; ///< Map of loaded chunks.
    std::unordered_set<glm::ivec3, utils::IVec3Hasher> m_pendingChunks; ///< Set of chunks pending generation.

    std::shared_ptr<concurrencpp::thread_pool_executor> m_chunkExecutor; ///< Thread pool of executors to generate chunks.
    std::shared_ptr<concurrencpp::manual_executor> m_mainExecutor; ///< Executor for the main thread.

    ChunkGenerator m_generator; ///< Procedural terrain generator.
};
} // namespace mc::world
