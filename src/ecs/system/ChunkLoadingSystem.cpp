#include "ecs/system/ChunkLoadingSystem.hpp"

#include "core/Logger.hpp"
#include "ecs/Ecs.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "world/World.hpp"

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
    float leftover = targetFrame - dt;
    m_timeBudget = (leftover > 0.0f ? leftover : 0.0f) * workFraction;

    auto opt = getCurrentChunk();
    if (!opt) return;

    if (*opt != m_lastCameraChunk)
    {
        m_lastCameraChunk = *opt;
        refillQueue(*opt);
    }

    auto start = clock::now();
    if (size_t launches = scheduleChunks(start))
    {
        updateStats(launches, start);
    }
}

std::optional<glm::ivec3> ChunkLoadingSystem::getCurrentChunk() const
{
    auto& transforms = m_ecs.getAllComponents<TransformComponent>();
    if (transforms.empty()) return std::nullopt;
    auto const& t = transforms.begin()->second.position;

    return glm::ivec3{
        static_cast<int>(std::floor(t.x / static_cast<float>(world::CHUNK_SIZE_X))),
        0,
        static_cast<int>(std::floor(t.z / static_cast<float>(world::CHUNK_SIZE_Z)))};
}

void ChunkLoadingSystem::refillQueue(glm::ivec3 currentChunk)
{
    m_loadQueue = {};

    float generateRadius = static_cast<float>(m_loadRadius) + 0.5f;
    float squaredGenerateRadius = generateRadius * generateRadius;

    std::vector<glm::ivec3> candidates;
    candidates.reserve((2 * m_loadRadius + 1) * (2 * m_loadRadius + 1));
    for (int x = -m_loadRadius; x <= m_loadRadius; ++x)
    {
        for (int z = -m_loadRadius; z <= m_loadRadius; ++z)
        {
            if (static_cast<float>(x * x + z * z) > squaredGenerateRadius)
                continue;
            glm::ivec3 pos = currentChunk + glm::ivec3{x, 0, z};
            if (!m_world.isChunkLoaded(pos) && !m_world.isChunkPending(pos))
            {
                candidates.push_back(pos);
            }
        }
    }

    std::sort(candidates.begin(), candidates.end(), [&](glm::ivec3 const& a, glm::ivec3 const& b) {
        int dxA = a.x - currentChunk.x;
        int dzA = a.z - currentChunk.z;
        int dxB = b.x - currentChunk.x;
        int dzB = b.z - currentChunk.z;
        return (dxA * dxA + dzA * dzA) < (dxB * dxB + dzB * dzB);
    });

    for (auto const& pos : candidates)
    {
        // LOG(DEBUG, "Queue chunk at [{}, {}]", pos.x, pos.z);
        m_loadQueue.push(pos);
    }
}

size_t ChunkLoadingSystem::scheduleChunks(time_point const& start)
{
    size_t launches = 0;
    while (!m_loadQueue.empty())
    {
        auto now = clock::now();
        if (std::chrono::duration<double>(now - start).count() >= m_timeBudget)
            break;
        glm::ivec3 pos = m_loadQueue.front();
        m_loadQueue.pop();

        m_world.loadChunk(pos).run();
        ++launches;
    }
    return launches;
}

void ChunkLoadingSystem::updateStats(size_t launches, time_point const& start)
{
    auto end = clock::now();
    double total = std::chrono::duration<double>(end - start).count();
    double avg = total / static_cast<double>(launches);

    m_avgScheduleTime = alpha * avg + (1.0 - alpha) * m_avgScheduleTime;
    LOG(DEBUG,
        "Scheduled {} chunks in {:.3f} ms  (EMA {:.3f} ms)",
        launches,
        total * 1000.0,
        m_avgScheduleTime * 1000.0);
}
} // namespace mc::ecs
