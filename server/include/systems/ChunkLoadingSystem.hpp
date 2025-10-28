#pragma once

#include <chrono>

#include <Magnum/Math/Vector3.h>
#include <ecs/system/ISystem.hpp>
#include <utils/PrioritizedChunk.hpp>
#include <utils/PriorityUniqueQueue.hpp>

namespace mc::world
{
class World;
}

namespace mc::ecs
{
class Ecs;

class ChunkLoadingSystem final : public ISystem
{
private:
    using clock = std::chrono::steady_clock;
    using time_point = clock::time_point;

public:
    ChunkLoadingSystem(Ecs& ecs, world::World& world, uint8_t radius);

    void update(float dt) override;

private:
    std::optional<Magnum::Vector3i> getCurrentChunk() const;
    void loadChunksInRadius(Magnum::Vector3i const& currentChunk);
    size_t processLoadQueue(time_point const& start);
    void updateStats(size_t launches, time_point const& start);

private:
    Ecs& m_ecs; ///< Reference to the ECS manager.
    world::World& m_world; ///< Reference to the ECS manager.

    uint8_t m_loadRadius; ///< Number of chunks to load around the current chunk.
    Magnum::Vector3i m_lastCameraChunk{std::numeric_limits<int>::max()}; ///< Tracks last known camera chunk position to avoid redundant loading.

    double m_avgScheduleTime = 0.0005; ///< Exponential moving average (EMA) of chunk scheduling time in seconds.
    double m_timeBudget = 0.0; ///< Maximum allowed time (in seconds) per frame for scheduling chunk loads.
    static constexpr double ALPHA = 0.1; ///< Smoothing factor for EMA calculation (closer to 1 = faster adaptation).
    static constexpr float WORK_FRACTION = 0.7f; ///< Fraction of leftover frame time allocated to chunk loading.

    utils::PriorityUniqueQueue<utils::PrioritizedChunk, utils::PrioritizedChunkHasher> m_loadQueue; ///< Queue of chunk positions awaiting generation.
};
} // namespace mc::ecs
