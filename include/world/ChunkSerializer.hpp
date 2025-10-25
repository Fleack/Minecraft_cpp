#pragma once

#include "world/Chunk.hpp"

#include <filesystem>
#include <memory>
#include <optional>

#include <Magnum/Math/Vector3.h>

namespace mc::world
{

/**
 * @brief Serializes and deserializes chunks to/from disk.
 *
 * This interface provides methods for persistent chunk storage.
 * Current implementation is a stub for future development.
 * 
 * TODO: Implement binary serialization format
 * TODO: Add compression support (e.g., LZ4, Zstandard)
 * TODO: Implement chunk versioning for backward compatibility
 * TODO: Add async I/O operations
 */
class ChunkSerializer
{
public:
    /**
     * @brief Saves a chunk to disk asynchronously.
     *
     * TODO: Implement actual serialization
     * Format considerations:
     * - Header: magic number, version, chunk position, timestamp
     * - Compressed block data
     * - Checksum for integrity validation
     *
     * @param chunk Chunk to save
     * @param worldPath Root directory for world save data
     */
    static void saveChunkAsync(Chunk const& chunk, std::filesystem::path const& worldPath);

    /**
     * @brief Loads a chunk from disk.
     *
     * TODO: Implement actual deserialization
     *
     * @param chunkPos Position of the chunk to load
     * @param worldPath Root directory for world save data
     * @return Unique pointer to loaded chunk, or nullptr if not found/failed
     */
    static std::unique_ptr<Chunk> loadChunk(Magnum::Vector3i const& chunkPos, std::filesystem::path const& worldPath);

    /**
     * @brief Checks if a chunk exists on disk.
     *
     * TODO: Implement file existence check with proper path construction
     *
     * @param chunkPos Position of the chunk
     * @param worldPath Root directory for world save data
     * @return True if the chunk file exists on disk
     */
    static bool chunkExistsOnDisk(Magnum::Vector3i const& chunkPos, std::filesystem::path const& worldPath);

private:
    /**
     * @brief Constructs the file path for a chunk.
     *
     * TODO: Implement region-based storage like Minecraft's Anvil format
     * Current idea: worlds/{worldName}/region/r.{regionX}.{regionZ}.dat
     * Each region file contains 32x32 chunks
     *
     * @param chunkPos Chunk position
     * @param worldPath World root directory
     * @return Path to the chunk file
     */
    static std::filesystem::path getChunkFilePath(Magnum::Vector3i const& chunkPos, std::filesystem::path const& worldPath);
};

} // namespace mc::world

