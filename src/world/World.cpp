#include "world/World.hpp"

#include "core/Logger.hpp"
#include "ecs/events/EventBus.hpp"
#include "ecs/events/Events.hpp"
#include "world/ChunkSerializer.hpp"

#include <ranges>

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

World::World(
    std::shared_ptr<concurrencpp::thread_pool_executor> chunkExecutor,
    ecs::EventBus& eventBus,
    int32_t seed)
    : m_chunkExecutor{std::move(chunkExecutor)}
    , m_eventBus{eventBus}
    , m_seed{seed}
    , m_generator{seed}
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
    
    m_eventBus.emit(ecs::ChunkLoaded{chunkPos});
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

int32_t World::getSeed() const
{
    return m_seed;
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

size_t World::unloadChunksOutsideRadius(Magnum::Vector3i const& centerChunk, uint8_t radius)
{
    auto chunksToUnload = findChunksToUnload(centerChunk, radius);
    
    if (chunksToUnload.empty())
    {
        return 0;
    }
    
    LOG(INFO, "Unloading {} chunks outside radius {}", chunksToUnload.size(), radius);
    
    for (auto const& chunkPos : chunksToUnload)
    {
        // TODO: Save dirty chunks to disk before unloading
        if (m_dirtyChunks.contains(chunkPos))
        {
            SPAM_LOG(DEBUG, "TODO: Save dirty chunk [{}, {}] to disk", chunkPos.x(), chunkPos.z());
            // ChunkSerializer::saveChunkAsync(*m_chunks[chunkPos], m_worldSavePath);
            m_dirtyChunks.erase(chunkPos);
        }
        
        // Remove chunk from memory
        m_chunks.erase(chunkPos);
        
        // Emit event so systems can clean up related data
        m_eventBus.emit(ecs::ChunkUnloaded{chunkPos});
        
        SPAM_LOG(DEBUG, "Unloaded chunk [{}, {}]", chunkPos.x(), chunkPos.z());
    }
    
    LOG(INFO, "Chunks remaining in memory: {}", m_chunks.size());
    return chunksToUnload.size();
}

std::vector<Magnum::Vector3i> World::findChunksToUnload(Magnum::Vector3i const& centerChunk, uint8_t radius) const
{
    std::vector<Magnum::Vector3i> toUnload;
    toUnload.reserve(m_chunks.size() / 4);
    
    auto const radiusSq = static_cast<float>(radius * radius);
    
    for (auto const& chunkPos : m_chunks | std::views::keys)
    {
        int const dx = chunkPos.x() - centerChunk.x();
        int const dz = chunkPos.z() - centerChunk.z();
        auto distanceSq = static_cast<float>(dx * dx + dz * dz);
        if (distanceSq > radiusSq)
        {
            toUnload.push_back(chunkPos);
        }
    }
    
    return toUnload;
}

size_t World::getLoadedChunkCount() const
{
    return m_chunks.size();
}

void World::markChunkDirty(Magnum::Vector3i const& chunkPos)
{
    if (m_chunks.contains(chunkPos))
    {
        m_dirtyChunks.insert(chunkPos);
        SPAM_LOG(DEBUG, "Marked chunk [{}, {}] as dirty", chunkPos.x(), chunkPos.z());
    }
}

} // namespace mc::world
