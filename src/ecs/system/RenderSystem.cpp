#include "ecs/system/RenderSystem.hpp"

#include "core/Logger.hpp"
#include "ecs/Ecs.hpp"
#include "ecs/component/MeshComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/system/CameraSystem.hpp"
#include "render/ChunkMeshBuilder.hpp"
#include "render/TextureManager.hpp"
#include "utils/FastDivFloor.hpp"
#include "world/Chunk.hpp"
#include "world/World.hpp"

#include <algorithm>

#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Matrix4.h>

namespace mc::ecs
{

RenderSystem::RenderSystem(
    Ecs& ecs,
    std::shared_ptr<concurrencpp::thread_pool_executor> meshExecutor,
    std::shared_ptr<CameraSystem> cameraSystem,
    world::World& world,
    uint8_t renderRadius)
    : ISystem{Type::RENDER}
    , m_ecs{ecs}
    , m_world{world}
    , m_meshExecutor{std::move(meshExecutor)}
    , m_cameraSystem{std::move(cameraSystem)}
    , m_renderRadius{renderRadius}
{
    m_visibleChunks.reserve((2 * m_renderRadius + 1) * (2 * m_renderRadius + 1));
    m_textureManager = std::make_unique<mc::render::TextureManager>("assets/textures/blocks");
    LOG(INFO, "RenderSystem initialized with render radius: {}", renderRadius);
}

void RenderSystem::update(float dt)
{
    constexpr float targetFrame = 1.0f / 60.0f;
    float leftover = targetFrame - dt;
    m_timeBudget = std::max(leftover, 0.0f) * workFraction;

    m_cachedCurrentChunk = getCurrentChunk();
    if (!m_cachedCurrentChunk) return;

    integrateFinishedMeshes();

    static Magnum::Vector3i lastChunk{0};
    if (*m_cachedCurrentChunk != lastChunk)
    {
        lastChunk = *m_cachedCurrentChunk;
        m_meshQueue.clear();
    }

    auto start = clock::now();
    if (size_t built = processMeshQueue(start))
    {
        updateStats(built, start);
    }
}

void RenderSystem::render(float)
{
    auto& transformComponents = m_ecs.getAllComponents<TransformComponent>();
    if (transformComponents.empty())
    {
        LOG(CRITICAL, "No TransformComponents found!");
        return;
    }
    auto playerPosition = transformComponents.begin()->second.position;

    constexpr float chunkSize = 16.0f;
    float renderDistanceInWorldUnits = 2 * m_renderRadius * chunkSize;

    m_shaderProgram
        // Basic render
        .setViewMatrix(m_cameraSystem->getViewMatrix())
        .setProjectionMatrix(m_cameraSystem->getProjectionMatrix())
        .setModelMatrix(Magnum::Matrix4{})
        // Lightning
        .setLightDirection({-1.0f, -0.3f, -1.0f})
        .setLightColor({1.0f, 1.0f, 0.9f})
        .setAmbientColor({0.7f, 0.7f, 0.8f})
        // Fog
        .setCameraPosition(static_cast<Magnum::Vector3>(playerPosition))
        .setFogColor({0.6f, 0.8f, 1.0f})
        .setFogDistance(renderDistanceInWorldUnits);

    if (m_cachedCurrentChunk)
    {
        drawChunksInRadius(*m_cachedCurrentChunk);
    }
}

std::optional<Magnum::Vector3i> RenderSystem::getCurrentChunk() const
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

void RenderSystem::drawChunksInRadius(Magnum::Vector3i const& currentChunkPos)
{
    constexpr auto sq = [](int x) { return x * x; };
    float const r = static_cast<float>(m_renderRadius) + 0.5f;
    float const r2 = r * r;

    m_visibleChunks.clear();
    for (int dz = -m_renderRadius; dz <= m_renderRadius; ++dz)
    {
        for (int dx = -m_renderRadius; dx <= m_renderRadius; ++dx)
        {
            if (sq(dx) + sq(dz) > r2) continue;
            m_visibleChunks.emplace(currentChunkPos.x() + dx, 0, currentChunkPos.z() + dz);
        }
    }

    for (auto const& pos : m_visibleChunks)
    {
        auto it = m_chunkToMesh.find(pos);
        if (it != m_chunkToMesh.end())
        {
            for (Entity e : it->second)
            {
                if (auto mc = m_ecs.getComponent<MeshComponent>(e))
                {
                    m_shaderProgram
                        .bindTexture(m_textureManager->get(mc->textureId))
                        .draw(*mc->mesh);
                }
            }
        }
        else
        {
            if (!m_cachedCurrentChunk) return;

            float dx = pos.x() - m_cachedCurrentChunk->x();
            float dz = pos.z() - m_cachedCurrentChunk->z();
            float distanceSq = dx * dx + dz * dz;

            enqueueChunkForMesh({pos, distanceSq});
        }
    }
}

void RenderSystem::enqueueChunkForMesh(utils::PrioritizedChunk const& chunk)
{
    if (m_meshQueue.contains(chunk) || m_pendingMeshes.contains(chunk.pos)) return;
    SPAM_LOG(DEBUG, "Enqueue mesh at [{}, {}] for generation", chunk.pos.x(), chunk.pos.z());
    m_meshQueue.push(chunk);
}

size_t RenderSystem::processMeshQueue(time_point const& start)
{
    size_t launches = 0;
    while (!m_meshQueue.empty())
    {
        if (std::chrono::duration<double>(clock::now() - start).count() >= m_timeBudget)
            break;

        auto chunk = m_meshQueue.pop();
        if (m_pendingMeshes.contains(chunk.pos)) continue;

        if (auto opt = m_world.getChunk(chunk.pos))
        {
            auto job = m_meshExecutor->submit([=, &world = m_world]() {
                return render::ChunkMeshBuilder::buildVertexData(opt->get(), world);
            });
            m_pendingMeshes.emplace(chunk.pos, std::move(job));
            ++launches;
        }
    }
    return launches;
}

void RenderSystem::updateStats(size_t launches, time_point const& start)
{
    double duration = std::chrono::duration<double>(clock::now() - start).count();
    double avg = duration / static_cast<double>(launches);
    m_avgBuildTime = alpha * avg + (1.0 - alpha) * m_avgBuildTime;

    // SPAM_LOG(DEBUG, "Built {} meshes in {:.3f} ms (EMA {:.3f} ms)", launches, duration * 1000.0, m_avgBuildTime * 1000.0);
}

void RenderSystem::integrateFinishedMeshes()
{
    std::vector<Magnum::Vector3i> toRemove;

    for (auto& [pos, job] : m_pendingMeshes)
    {
        if (!job)
        {
            toRemove.push_back(pos);
            continue;
        }

        if (job.status() != concurrencpp::result_status::value) continue;

        auto vertsByTex = job.get();
        auto blocksMeshes = render::ChunkMeshBuilder::buildMeshComponents(vertsByTex);

        SPAM_LOG(DEBUG, "Commiting mesh [{}, {}] into mesh map", pos.x(), pos.z());
        auto& vec = m_chunkToMesh[pos];
        for (auto& mesh : blocksMeshes)
        {
            Entity e = m_ecs.createEntity();
            m_ecs.addComponent<MeshComponent>(e, mesh);
            vec.push_back(e);
        }
    }

    for (auto& pos : toRemove)
    {
        m_pendingMeshes.erase(pos);
    }
}
} // namespace mc::ecs
