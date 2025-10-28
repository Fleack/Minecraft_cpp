#include "world/SimpleChunkProvider.hpp"

#include <core/Logger.hpp>

namespace mc::world
{

SimpleChunkProvider::SimpleChunkProvider()
    : m_chunk{{0, 0, 0}}
{
    for (int x = 0; x < CHUNK_SIZE_X; ++x)
    {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z)
        {
            m_chunk.setBlock(x, 0, z, {BlockType::STONE});
            m_chunk.setBlock(x, 1, z, {BlockType::STONE});
        }
    }

    LOG(INFO, "SimpleChunkProvider initialized with flat chunk at (0, 0, 0)");
}

std::optional<std::reference_wrapper<Chunk const>> SimpleChunkProvider::getChunk(Magnum::Vector3i const& chunkPos) const
{
    if (chunkPos.y() == 0)
    {
        return std::cref(m_chunk);
    }
    return std::nullopt;
}

} // namespace mc::world

