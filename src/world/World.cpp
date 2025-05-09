#include "world/World.hpp"

#include "core/Logger.hpp"

namespace mc::world
{

World::World(std::shared_ptr<concurrencpp::thread_pool_executor> chunkExecutor)
    : m_chunkExecutor{std::move(chunkExecutor)}
{}

concurrencpp::lazy_result<void> World::loadChunk(Magnum::Vector3i chunkPos)
{
    if (m_chunks.contains(chunkPos) || m_pendingChunks.contains(chunkPos))
    {
        co_return;
    }
    enqueueChunk(chunkPos);
    auto chunk = co_await generateChunkAsync(chunkPos);
    commitChunk(chunkPos, std::move(chunk));
}

std::optional<std::reference_wrapper<Chunk>> World::getChunk(Magnum::Vector3i const& chunkPos) const
{
    auto it = m_chunks.find(chunkPos);
    if (it != m_chunks.end())
    {
        return std::ref(*it->second);
    }
    return std::nullopt;
}

void World::enqueueChunk(Magnum::Vector3i const& chunkPos)
{
    SPAM_LOG(DEBUG, "Enqueue chunk at [{}, {}] for generation", chunkPos.x(), chunkPos.z());
    m_pendingChunks.insert(chunkPos);
}

concurrencpp::lazy_result<std::unique_ptr<Chunk>> World::generateChunkAsync(Magnum::Vector3i chunkPos) const
{
    return m_generator.generate(chunkPos, m_chunkExecutor);
}

void World::commitChunk(Magnum::Vector3i chunkPos, std::unique_ptr<Chunk> chunkPtr)
{
    SPAM_LOG(INFO, "Committing chunk [{}, {}] into final map", chunkPos.x(), chunkPos.z());
    m_chunks[chunkPos] = std::move(chunkPtr);
    m_pendingChunks.erase(chunkPos);
}

bool World::isChunkLoaded(Magnum::Vector3i const& pos) const
{
    return m_chunks.contains(pos);
}

bool World::isChunkPending(Magnum::Vector3i const& pos) const
{
    return m_pendingChunks.contains(pos);
}

} // namespace mc::world
