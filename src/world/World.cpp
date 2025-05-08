#include "world/World.hpp"

#include "core/Logger.hpp"

namespace mc::world
{

World::World(
    std::shared_ptr<concurrencpp::thread_pool_executor> chunkExecutor,
    std::shared_ptr<concurrencpp::manual_executor> mainExec)
    : m_chunkExecutor{std::move(chunkExecutor)}, m_mainExecutor{std::move(mainExec)}
{}

concurrencpp::lazy_result<void> World::loadChunk(Magnum::Vector3i chunkPos)
{
    if (m_chunks.contains(chunkPos) || m_pendingChunks.contains(chunkPos))
    {
        co_return;
    }
    enqueueChunk(chunkPos);
    auto chunk = co_await generateChunkAsync(chunkPos);
    co_await commitChunk(chunkPos, std::move(chunk));
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
    co_await concurrencpp::resume_on(m_chunkExecutor);
    auto chunk = std::make_unique<Chunk>(chunkPos);
    co_await m_generator.generate(*chunk, m_chunkExecutor);
    co_return chunk;
}

concurrencpp::lazy_result<void> World::commitChunk(Magnum::Vector3i chunkPos, std::unique_ptr<Chunk> chunkPtr)
{
    co_await concurrencpp::resume_on(m_mainExecutor);
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
