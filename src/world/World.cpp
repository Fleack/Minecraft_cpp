#include "world/World.hpp"

#include "core/Logger.hpp"

#include <Magnum/Math/Functions.h>

namespace
{
constexpr int div_floor(int a, int b)
{
    return (a < 0) ? ((a - b + 1) / b) : (a / b);
}
} // namespace

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

std::unordered_map<Magnum::Vector3i, std::unique_ptr<Chunk>, utils::IVec3Hasher> const& World::getChunks() const
{
    return m_chunks;
}

std::unordered_set<Magnum::Vector3i, utils::IVec3Hasher> const& World::getPendingChunks() const
{
    return m_pendingChunks;
}

Magnum::Vector3i World::getChunkOfPosition(Magnum::Vector3i const& position)
{
    return {
        div_floor(position.x(), CHUNK_SIZE_X),
        0,
        div_floor(position.z(), CHUNK_SIZE_Z)};
}

Magnum::Vector3i World::getChunkOfPosition(Magnum::Vector3d const& position)
{
    return getChunkOfPosition(Magnum::Vector3i{Magnum::Math::floor(position)});
}

} // namespace mc::world
