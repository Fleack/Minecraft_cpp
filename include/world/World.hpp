#pragma once

#include "utils/IVer3Hasher.hpp"
#include "world/ChunkGenerator.hpp"

#include <memory>
#include <optional>
#include <unordered_map>
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
    World() = default;

    /**
     * @brief Loads and generates a chunk at the given chunk-space position.
     *
     * If the chunk is already loaded, this function does nothing.
     *
     * @param chunkPos Position of the chunk in chunk-space coordinates.
     */
    void loadChunk(glm::ivec3 const& chunkPos);

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

    [[nodiscard]] auto const& getChunks() const { return m_chunks; }

    /**
     * @brief Generates a square area of chunks centered at origin.
     *
     * Useful during world initialization to pre-load visible terrain.
     *
     * @param radius Number of chunks to load in each direction (X and Z).
     */
    void generateInitialArea(int radius = 1);

private:
    std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>, utils::IVec3Hasher> m_chunks; ///< Map of loaded chunks.
    ChunkGenerator m_generator; ///< Procedural terrain generator.
};
}
