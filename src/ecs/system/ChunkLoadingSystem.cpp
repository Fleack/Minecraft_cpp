#include "ecs/system/ChunkLoadingSystem.hpp"

#include "core/Logger.hpp"
#include "ecs/Ecs.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "utils/FastDivFloor.hpp"
#include "world/World.hpp"

#include <algorithm>
#include <cmath>

namespace mc::ecs
{

ChunkLoadingSystem::ChunkLoadingSystem(Ecs& ecs, world::World& world, uint8_t radius)
    : ISystem{Type::CHUNK_LOADING}, m_ecs(ecs), m_world(world), m_loadRadius(radius)
{
    LOG(INFO, "ChunkLoadingSystem initialized with load radius: {}", radius);
}

void ChunkLoadingSystem::update(float dt)
{
    constexpr float targetFrame = 1.0f / 60.0f;
    float const leftover = targetFrame - dt;
    m_timeBudget = std::max(0.001f, leftover) * workFraction;

    auto currentChunk = getCurrentChunk();
    if (!currentChunk) return;

    if (*currentChunk != m_lastCameraChunk)
    {
        m_lastCameraChunk = *currentChunk;
        loadChunksInRadius(*currentChunk);
    }

    auto start = clock::now();
    if (size_t launches = processLoadQueue(start))
    {
        updateStats(launches, start);
    }
    m_world.integrateFinishedChunks();
}

std::optional<Magnum::Vector3i> ChunkLoadingSystem::getCurrentChunk() const
{
    auto& transforms = m_ecs.getAllComponents<TransformComponent>();
    if (transforms.empty())
    {
        LOG(CRITICAL, "No TransformComponents found!");
        return std::nullopt;
    }

    auto const& pos = transforms.begin()->second.position;
    int const cx = utils::floor_div(pos.x(), world::CHUNK_SIZE_X);
    int const cz = utils::floor_div(pos.z(), world::CHUNK_SIZE_Z);

    return Magnum::Vector3i{cx, 0, cz};
}

void ChunkLoadingSystem::loadChunksInRadius(Magnum::Vector3i const& currentChunk)
{
    m_loadQueue.clear();
    float radius = static_cast<float>(m_loadRadius) + 0.5f;
    float radiusSq = radius * radius;

    tsl::hopscotch_set<Magnum::Vector3i, utils::IVec3Hasher> candidates;
    candidates.reserve((2 * m_loadRadius + 1) * (2 * m_loadRadius + 1));
    for (int x = -m_loadRadius; x <= m_loadRadius; ++x)
    {
        for (int z = -m_loadRadius; z <= m_loadRadius; ++z)
        {
            if (static_cast<float>(x * x + z * z) > radiusSq)
                continue;

            Magnum::Vector3i pos = currentChunk + Magnum::Vector3i{x, 0, z};
            if (!m_world.isChunkLoaded(pos) && !m_world.isChunkPending(pos))
                candidates.insert(pos);
        }
    }

    for (auto const& pos : candidates)
    {
        float dx = pos.x() - currentChunk.x();
        float dz = pos.z() - currentChunk.z();
        float distanceSq = dx * dx + dz * dz;

        m_loadQueue.push({pos, distanceSq});
    }
}

size_t ChunkLoadingSystem::processLoadQueue(time_point const& start)
{
    size_t launches = 0;
    while (!m_loadQueue.empty())
    {
        if (std::chrono::duration<double>(clock::now() - start).count() >= m_timeBudget)
            break;

        auto chunk = m_loadQueue.pop();
        if (!chunk) break;

        m_world.submitChunkLoad(chunk->pos);
        ++launches;
    }
    return launches;
}

void ChunkLoadingSystem::updateStats(size_t launches, time_point const& start)
{
    auto const duration = std::chrono::duration<double>(clock::now() - start).count();
    double const avg = duration / static_cast<double>(launches);
    m_avgScheduleTime = alpha * avg + (1.0 - alpha) * m_avgScheduleTime;

    SPAM_LOG(DEBUG, "Scheduled {} chunks in {:.3f} ms (EMA {:.3f} ms)", launches, duration * 1000.0, m_avgScheduleTime * 1000.0);
}

} // namespace mc::ecs
