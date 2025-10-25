#include "world/ChunkSerializer.hpp"

#include "core/Logger.hpp"

namespace mc::world
{

void ChunkSerializer::saveChunkAsync(Chunk const&, std::filesystem::path const&)
{
    // TODO: Implement async chunk saving
    // 1. Serialize chunk data to binary format
    // 2. Compress the data
    // 3. Write to disk asynchronously
    // 4. Add error handling and logging

    // auto const& pos = chunk.getPosition();
    SPAM_LOG(DEBUG, "TODO: Save chunk [{}, {}, {}] to {}", pos.x(), pos.y(), pos.z(), worldPath.string());
}

std::unique_ptr<Chunk> ChunkSerializer::loadChunk(Magnum::Vector3i const&, std::filesystem::path const&)
{
    // TODO: Implement chunk loading from disk
    // 1. Check if file exists
    // 2. Read file data
    // 3. Decompress if needed
    // 4. Deserialize into Chunk object
    // 5. Validate data integrity (checksum)

    SPAM_LOG(DEBUG, "TODO: Load chunk [{}, {}, {}] from {}", chunkPos.x(), chunkPos.y(), chunkPos.z(), worldPath.string());
    return nullptr;
}

bool ChunkSerializer::chunkExistsOnDisk(Magnum::Vector3i const&, std::filesystem::path const&)
{
    // TODO: Implement file existence check
    // auto filePath = getChunkFilePath(chunkPos, worldPath);
    // return std::filesystem::exists(filePath);

    SPAM_LOG(DEBUG, "TODO: Check if chunk [{}, {}, {}] exists at {}", chunkPos.x(), chunkPos.y(), chunkPos.z(), worldPath.string());
    return false;
}

std::filesystem::path ChunkSerializer::getChunkFilePath(Magnum::Vector3i const&, std::filesystem::path const& worldPath)
{
    // TODO: Implement proper path construction
    // Consider region-based storage format:
    // Region contains 32x32 chunks (like Minecraft Anvil format)
    // File: worlds/{worldName}/region/r.{regionX}.{regionZ}.dat
    
    // Example implementation (commented out):
    // int regionX = chunkPos.x() >> 5; // divide by 32
    // int regionZ = chunkPos.z() >> 5;
    // return worldPath / "region" / fmt::format("r.{}.{}.dat", regionX, regionZ);

    return worldPath / "chunks" / "placeholder.dat";
}

} // namespace mc::world

