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
    m_pendingChunks.insert(chunkPos);
}

void World::submitChunkLoad(Magnum::Vector3i const& chunkPos)
{
    if (m_chunks.contains(chunkPos) || m_pendingChunks.contains(chunkPos))
        return;

    enqueueChunk(chunkPos);

    auto job = m_chunkExecutor->submit([=, this]() {
        SPAM_LOG(DEBUG, "Enqueue chunk at [{}, {}] for generation on thread {}", chunkPos.x(), chunkPos.z(), std::this_thread::get_id());
        return m_generator.generate(chunkPos);
    });

    m_pendingChunkResults[chunkPos] = std::move(job);
}

void World::integrateFinishedChunks()
{
    std::vector<Magnum::Vector3i> toRemove;
    toRemove.reserve(m_pendingChunkResults.size());
    for (auto& [pos, result] : m_pendingChunkResults)
    {
        if (result.status() != concurrencpp::result_status::value)
            continue;

        auto chunk = result.get();
        commitChunk(pos, std::move(chunk));
        toRemove.push_back(pos);
    }

    for (auto const& pos : toRemove)
    {
        m_pendingChunkResults.erase(pos);
    }
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
