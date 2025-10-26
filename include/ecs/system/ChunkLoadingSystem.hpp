#pragma once

#include "ecs/system/ISystem.hpp"
#include "utils/PrioritizedChunk.hpp"
#include "utils/PriorityUniqueQueue.hpp"

#include <chrono>

#include <Magnum/Math/Vector3.h>

namespace mc::world
{
class World;
}

namespace mc::ecs
{
class Ecs;

/**
 * @brief ECS system for dynamically loading chunks around the camera.
 *
 * Tracks the camera position and loads/generates chunks within a configurable radius,
 * respecting a frame time budget to maintain stable performance.
 */
class ChunkLoadingSystem final : public ISystem
{
private:
    using clock = std::chrono::steady_clock;
    using time_point = clock::time_point;

public:
    /**
     * @brief Constructs the ChunkLoadingSystem.
     *
     * @param ecs Reference to the ECS manager.
     * @param world Reference to the world managing chunks.
     * @param radius Radius (in chunks) around the camera to maintain loaded.
     */
    ChunkLoadingSystem(Ecs& ecs, world::World& world, uint8_t radius);

    /**
     * @brief Updates the chunk loading system each frame.
     *
     * Recomputes the available time budget based on frame delta time.
     * If the camera moves, refills the chunk load queue.
     * Then schedules chunk loading within the available time.
     *
     * @param dt Delta time in seconds since the last frame.
     */
    void update(float dt) override;

    /**
     * @brief Does nothing.
     */
    void render(float) override {}

private:
    /**
     * @brief Gets the current chunk-space position of the camera.
     *
     * @return Optional chunk-space position, or std::nullopt if no transform exists.
     */
    std::optional<Magnum::Vector3i> getCurrentChunk() const;

    /**
     * @brief Refills the loading queue with chunks around the current camera chunk.
     *
     * Only chunks that are not yet loaded or pending are enqueued.
     *
     * @param currentChunk Current camera chunk position.
     */
    void loadChunksInRadius(Magnum::Vector3i const& currentChunk);

    /**
     * @brief Schedules chunk loading operations within the time budget.
     *
     * Runs chunk generation immediately on the calling thread and
     * stops scheduling if the time budget is exceeded.
     *
     * @param start Start time point of scheduling batch.
     * @return Number of chunks launched for loading.
     */
    size_t processLoadQueue(time_point const& start);

    /**
     * @brief Updates the statistics on chunk scheduling performance.
     *
     * Calculates the total scheduling time and updates an exponential moving average (EMA)
     * to monitor performance over time.
     *
     * @param launches Number of chunks scheduled during this frame.
     * @param start Start time point of scheduling batch.
     */
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
