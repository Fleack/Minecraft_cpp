#include "world/World.hpp"

#include "core/Logger.hpp"

namespace mc::world
{
World::World(std::shared_ptr<concurrencpp::thread_pool_executor> chunkExecutor) : m_chunkExecutor{chunkExecutor} {}

concurrencpp::result<void> World::loadChunk(const glm::ivec3& chunkPos)
{
    if (isChunkLoaded(chunkPos) || isChunkPending(chunkPos))
    {
        return concurrencpp::make_ready_result<void>();
    }

    LOG(INFO, "Loading new chunk at [{} , {}]", chunkPos.x, chunkPos.z);
    m_pendingChunks.emplace(chunkPos, std::make_unique<Chunk>(chunkPos));
    auto generationResult = m_generator.generate(*m_pendingChunks[chunkPos], m_chunkExecutor).run();

    concurrencpp::result_promise<void> promise;
    auto finalResult = promise.get_result();

    m_chunkExecutor->post([this, chunkPos, genResult = std::move(generationResult), promise = std::move(promise)]() mutable {
        try
        {
            genResult.get();
            m_chunks[chunkPos] = std::move(m_pendingChunks[chunkPos]);
            m_pendingChunks.erase(chunkPos);
            LOG(INFO, "Chunk generation succeeded at [{}, {}]", chunkPos.x, chunkPos.z);
            promise.set_result();
        }
        catch (const std::exception& e)
        {
            LOG(ERROR, "Chunk generation failed at [{}, {}]: {}", chunkPos.x, chunkPos.z, e.what());
            promise.set_exception(std::current_exception());
        }
    });

    return finalResult;
}

std::optional<std::reference_wrapper<Chunk>> World::getChunk(glm::ivec3 const& chunkPos)
{
    auto it = m_chunks.find(chunkPos);
    if (it != m_chunks.end())
    {
        return std::ref(*it->second);
    }
    return std::nullopt;
}

bool World::isChunkLoaded(glm::ivec3 const& pos) const
{
    return m_chunks.contains(pos);
}

bool World::isChunkPending(glm::ivec3 const& pos) const
{
    return m_pendingChunks.contains(pos);
}
} // namespace mc::world
