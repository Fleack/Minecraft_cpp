#pragma once

#include "ecs/Entity.hpp"
#include "ecs/component/MeshComponent.hpp"
#include "ecs/system/ISystem.hpp"
#include "render/ShaderProgram.hpp"
#include "render/TextureManager.hpp"
#include "render/Vertex.hpp"
#include "utils/IVec3Hasher.hpp"
#include "utils/PrioritizedChunk.hpp"
#include "utils/PriorityUniqueQueue.hpp"
#include "utils/UniqueQueue.hpp"

#include <chrono>
#include <memory>

#include <Magnum/Math/Vector3.h>
#include <concurrencpp/executors/thread_pool_executor.h>

namespace mc::world
{
class World;
}

namespace mc::ecs
{
class Ecs;
}

namespace mc::render
{
class ShaderProgram;
} // namespace mc::render

namespace mc::ecs
{
class CameraSystem;

/**
 * @brief ECS system responsible for rendering voxel chunks.
 *
 * Handles chunk mesh generation, caching, and OpenGL rendering.
 * Builds chunk meshes around the camera within a configurable radius
 * and manages per-frame build time to avoid frame time spikes.
 */
class RenderSystem final : public ISystem
{
private:
    using clock = std::chrono::steady_clock;
    using time_point = clock::time_point;
    using chunk_mesh_job = concurrencpp::result<std::vector<std::vector<render::Vertex>>>;

public:
    /**
     * @brief Constructs the RenderSystem.
     *
     * @param ecs Reference to the ECS manager.
     * @param cameraSystem Shared pointer to the camera system.
     * @param world Reference to the world managing chunks.
     * @param renderRadius Radius (in chunks) to render around the camera.
     */
    RenderSystem(
        Ecs& ecs,
        std::shared_ptr<concurrencpp::thread_pool_executor> meshExecutor,
        std::shared_ptr<CameraSystem> cameraSystem,
        world::World& world,
        uint8_t renderRadius);

    /**
     * @brief Updates the render system.
     *
     * Computes the available time budget, detects camera movement, and processes the chunk mesh build queue.
     *
     * @param dt Delta time since the last frame (in seconds).
     */
    void update(float dt) override;

    /**
     * @brief Renders all visible chunks.
     *
     * Binds the shader and texture atlas, sets view and projection matrices,
     * and draws all chunk meshes within the render radius.
     */
    void render(float) override;

private:
    /**
     * @brief Gets the current chunk position based on the camera.
     *
     * @return Optional chunk-space position if a transform exists, otherwise std::nullopt.
     */
    std::optional<Magnum::Vector3i> getCurrentChunk() const;

    /**
     * @brief Draws all chunks within the configured render radius.
     *
     * Enqueues chunks for mesh building if needed and renders already-built meshes.
     *
     * @param currentChunkPos Camera's current chunk-space position.
     */
    void drawChunksInRadius(Magnum::Vector3i const& currentChunkPos);

    /**
     * @brief Enqueues a chunk for mesh generation if it is not already enqueued.
     *
     * @param chunk
     */
    void enqueueChunkForMesh(utils::PrioritizedChunk const& chunk);

    /**
     * @brief Processes the chunk mesh generation queue within the frame's time budget.
     *
     * Builds new chunk meshes if the chunk exists, otherwise re-enqueues them.
     *
     * @param start Start time of this processing batch.
     * @return Number of meshes built during this frame.
     */
    size_t processMeshQueue(time_point const& start);

    /**
     * @brief Updates statistics about mesh building performance.
     *
     * Updates an exponential moving average (EMA) of chunk mesh build times.
     *
     * @param launches Number of meshes built in this frame.
     * @param start Start time of the build batch.
     */
    void updateStats(size_t launches, time_point const& start);

    void integrateFinishedMeshes();

private:
    Ecs& m_ecs; ///< ECS manager reference.
    world::World& m_world; ///< Reference to the world for chunk access.
    render::ShaderProgram m_shaderProgram{}; ///< Shader program used for rendering.
    std::unique_ptr<render::TextureManager> m_textureManager;
    std::shared_ptr<concurrencpp::thread_pool_executor> m_meshExecutor;

    std::shared_ptr<CameraSystem> m_cameraSystem; ///< Provides view and projection matrices.

    uint8_t m_renderRadius; ///< Radius (in chunks) for rendering around the camera.
    double m_avgBuildTime = 0.002; ///< Exponential moving average (EMA) of mesh build time in seconds.
    double m_timeBudget = 0.0; ///< Maximum allowed time (in seconds) per frame for scheduling chunk loads.
    static constexpr double alpha = 0.1; ///< Smoothing factor for EMA calculation.
    static constexpr float workFraction = 0.3f; ///< Fraction of leftover frame time allowed for mesh building.

    std::unordered_map<Magnum::Vector3i, chunk_mesh_job, utils::IVec3Hasher> m_pendingMeshes;
    utils::PriorityUniqueQueue<utils::PrioritizedChunk, utils::PrioritizedChunkHasher> m_meshQueue;
    std::unordered_map<Magnum::Vector3i, std::vector<Entity>, utils::IVec3Hasher> m_chunkToMesh;

    std::optional<Magnum::Vector3i> m_cachedCurrentChunk;
    tsl::hopscotch_set<Magnum::Vector3i, utils::IVec3Hasher> m_visibleChunks;
};
} // namespace mc::ecs
