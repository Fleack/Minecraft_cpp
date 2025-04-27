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
     * @brief Loads and generates a chunk at the given chunk-space position.
     *
     * If the chunk is already loaded or pending to be loaded,
     * this function does nothing.
     *
     * @param chunkPos Position of the chunk in chunk-space coordinates.
     *
     * @return A result which is available after chunk generated and loaded
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
    void enqueueChunk(glm::ivec3 chunkPos);
    concurrencpp::lazy_result<std::unique_ptr<Chunk>> generateChunkAsync(glm::ivec3 chunkPos) const;
    concurrencpp::lazy_result<void> commitChunkAsync(glm::ivec3 chunkPos, std::unique_ptr<Chunk> chunkPtr);

private:
    std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>, utils::IVec3Hasher> m_chunks; ///< Map of loaded chunks.
    std::unordered_set<glm::ivec3, utils::IVec3Hasher> m_pendingChunks; ///< Map of chunks pending generation

    std::shared_ptr<concurrencpp::thread_pool_executor> m_chunkExecutor; ///< Thread pool of executors to generate chunks
    std::shared_ptr<concurrencpp::manual_executor> m_mainExecutor; ///< Executor for main thread

    ChunkGenerator m_generator; ///< Procedural terrain generator.
};
} // namespace mc::world
