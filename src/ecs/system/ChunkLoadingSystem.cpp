#include "ecs/system/ChunkLoadingSystem.hpp"

#include "core/Logger.hpp"
#include "ecs/Ecs.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "world/World.hpp"

#include <algorithm>
#include <cmath>
#include <ska_sort.hpp>

namespace mc::ecs
{

ChunkLoadingSystem::ChunkLoadingSystem(Ecs& ecs, world::World& world, uint8_t radius)
    : m_ecs(ecs), m_world(world), m_loadRadius(radius)
{
    LOG(INFO, "ChunkLoadingSystem initialized with load radius: {}", radius);
}

void ChunkLoadingSystem::update(float dt)
{
    constexpr float targetFrame = 1.0f / 60.0f;
    float const leftover = targetFrame - dt;
    m_timeBudget = (leftover > 0.0f ? leftover : 0.0f) * workFraction;

    auto currentChunk = getCurrentChunk();
    if (!currentChunk) return;

    if (*currentChunk != m_lastCameraChunk)
    {
        m_lastCameraChunk = *currentChunk;
        refillQueue(*currentChunk);
    }

    auto start = clock::now();
    if (size_t launches = scheduleChunks(start))
    {
        updateStats(launches, start);
    }
}

std::optional<Magnum::Math::Vector3<int>> ChunkLoadingSystem::getCurrentChunk() const
{
    auto& transforms = m_ecs.getAllComponents<TransformComponent>();
    if (transforms.empty()) return std::nullopt;

    auto const& pos = transforms.begin()->second.position;
    int const cx = static_cast<int>(std::floor(pos.x() / static_cast<float>(world::CHUNK_SIZE_X)));
    int const cz = static_cast<int>(std::floor(pos.z() / static_cast<float>(world::CHUNK_SIZE_Z)));

    return Magnum::Math::Vector3<int>{cx, 0, cz};
}

void ChunkLoadingSystem::refillQueue(Magnum::Math::Vector3<int> currentChunk)
{
    m_loadQueue = {};
    float radius = static_cast<float>(m_loadRadius) + 0.5f;
    float radiusSq = radius * radius;

    std::vector<Magnum::Math::Vector3<int>> candidates;
    for (int x = -m_loadRadius; x <= m_loadRadius; ++x)
    {
        for (int z = -m_loadRadius; z <= m_loadRadius; ++z)
        {
            if (static_cast<float>(x * x + z * z) > radiusSq)
                continue;

            Magnum::Math::Vector3<int> pos = currentChunk + Magnum::Math::Vector3<int>{x, 0, z};
            if (!m_world.isChunkLoaded(pos) && !m_world.isChunkPending(pos))
                candidates.push_back(pos);
        }
    }

    ska_sort(candidates.begin(), candidates.end(), [currentChunk](auto const& a) {
        return sq(a.x() - currentChunk.x()) + sq(a.z() - currentChunk.z());
    });

    for (auto const& pos : candidates)
    {
        SPAM_LOG(DEBUG, "Enqueue chunk at [{}, {}] for render", pos.x(), pos.z());
        m_loadQueue.push(pos);
    }
}

size_t ChunkLoadingSystem::scheduleChunks(time_point const& start)
{
    size_t launches = 0;
    while (!m_loadQueue.empty())
    {
        if (std::chrono::duration<double>(clock::now() - start).count() >= m_timeBudget)
            break;

        Magnum::Math::Vector3<int> const pos = m_loadQueue.front();
        m_loadQueue.pop();

        m_world.loadChunk(pos).run();
        ++launches;
    }
    return launches;
}

void ChunkLoadingSystem::updateStats(size_t launches, time_point const& start)
{
    auto const duration = std::chrono::duration<double>(clock::now() - start).count();
    double const avg = duration / static_cast<double>(launches);
    m_avgScheduleTime = alpha * avg + (1.0 - alpha) * m_avgScheduleTime;

    LOG(DEBUG,
        "Scheduled {} chunks in {:.3f} ms (EMA {:.3f} ms)",
        launches,
        duration * 1000.0,
        m_avgScheduleTime * 1000.0);
}

} // namespace mc::ecs
