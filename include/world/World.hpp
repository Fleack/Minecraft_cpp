#pragma once

#include "world/ChunkGenerator.hpp"

#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include <Magnum/Math/Vector3.h>

#include "utils/IVec3Hasher.hpp"

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
    World(
        std::shared_ptr<concurrencpp::thread_pool_executor> chunkExecutor,
        std::shared_ptr<concurrencpp::manual_executor> mainExec);

    /**
     * @brief Asynchronously loads a chunk if not already loaded or pending.
     *
     * Enqueues and generates the chunk asynchronously if it isn't present.
     * @param chunkPos Position of the chunk in chunk-space.
     * @return Lazy result completing when the chunk is ready.
     */
    concurrencpp::lazy_result<void> loadChunk(Magnum::Math::Vector3<int> chunkPos);

    /**
     * @brief Retrieves a loaded chunk by its position.
     *
     * @param chunkPos Position of the chunk in chunk-space.
     * @return Optional reference to the chunk if loaded.
     */
    [[nodiscard]] std::optional<std::reference_wrapper<Chunk>> getChunk(Magnum::Math::Vector3<int> const& chunkPos);

    /**
     * @brief Checks whether a chunk is currently loaded.
     *
     * @param pos Chunk position to check.
     * @return True if the chunk is present in memory.
     */
    [[nodiscard]] bool isChunkLoaded(Magnum::Math::Vector3<int> const& pos) const;

    /**
     * @brief Checks whether a chunk is currently pending to be loaded.
     *
     * @param pos Chunk position to check.
     * @return True if the chunk is a pending generation.
     */
    [[nodiscard]] bool isChunkPending(Magnum::Math::Vector3<int> const& pos) const;

    [[nodiscard]] auto const& getChunks() const
    {
        return m_chunks;
    }

    [[nodiscard]] auto const& getPendingChunks() const
    {
        return m_pendingChunks;
    }

private:
    void enqueueChunk(Magnum::Math::Vector3<int> const& chunkPos);

    concurrencpp::lazy_result<std::unique_ptr<Chunk>> generateChunkAsync(Magnum::Math::Vector3<int> chunkPos) const;
    concurrencpp::lazy_result<void> commitChunkAsync(Magnum::Math::Vector3<int> chunkPos, std::unique_ptr<Chunk> chunkPtr);

private:
    std::unordered_map<Magnum::Math::Vector3<int>, std::unique_ptr<Chunk>, utils::IVec3Hasher> m_chunks;
    std::unordered_set<Magnum::Math::Vector3<int>, utils::IVec3Hasher> m_pendingChunks;

    std::shared_ptr<concurrencpp::thread_pool_executor> m_chunkExecutor;
    std::shared_ptr<concurrencpp::manual_executor> m_mainExecutor;
    ChunkGenerator m_generator;
};

} // namespace mc::world
