#include "ecs/system/RenderSystem.hpp"

#include "core/Logger.hpp"
#include "ecs/Ecs.hpp"
#include "ecs/component/MeshComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/system/CameraSystem.hpp"
#include "render/ChunkMeshBuilder.hpp"
#include "render/TextureManager.hpp"
#include "world/Chunk.hpp"
#include "world/World.hpp"

#include <algorithm>

#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Matrix4.h>

namespace mc::ecs
{

RenderSystem::RenderSystem(
    Ecs& ecs,
    std::shared_ptr<CameraSystem> cameraSystem,
    world::World& world,
    uint8_t renderRadius)
    : m_ecs{ecs}, m_world{world}, m_cameraSystem{std::move(cameraSystem)}, m_renderRadius{renderRadius}
{
    m_textureManager = std::make_unique<mc::render::TextureManager>("assets/textures/blocks");
    LOG(INFO, "RenderSystem initialized with render radius: {}", renderRadius);
}

void RenderSystem::update(float dt)
{
    constexpr float targetFrame = 1.0f / 60.0f;
    float leftover = targetFrame - dt;
    m_timeBudget = std::max(leftover, 0.0f) * workFraction;

    auto current = getCurrentChunk();
    if (!current) return;

    static Magnum::Math::Vector3<int> lastChunk{0};
    if (*current != lastChunk)
    {
        lastChunk = *current;
        m_meshQueue = {};
        m_enqueuedChunks.clear();
    }

    auto start = clock::now();
    if (size_t built = processMeshQueue(start))
    {
        updateStats(built, start);
    }
}

void RenderSystem::render()
{
    m_shaderProgram
        .setViewMatrix(m_cameraSystem->getViewMatrix())
        .setProjectionMatrix(m_cameraSystem->getProjectionMatrix())
        .setModelMatrix(Magnum::Matrix4{});

    if (auto currentChunk = getCurrentChunk())
    {
        drawChunksInRadius(*currentChunk);
    }
}

std::optional<Magnum::Math::Vector3<int>> RenderSystem::getCurrentChunk() const
{
    auto& transforms = m_ecs.getAllComponents<TransformComponent>();
    if (transforms.empty()) return std::nullopt;

    auto const& pos = transforms.begin()->second.position;
    int const cx = static_cast<int>(std::floor(pos.x() / static_cast<float>(world::CHUNK_SIZE_X)));
    int const cz = static_cast<int>(std::floor(pos.z() / static_cast<float>(world::CHUNK_SIZE_Z)));

    return Magnum::Math::Vector3<int>{cx, 0, cz};
}

void RenderSystem::drawChunksInRadius(Magnum::Math::Vector3<int> const& currentChunkPos)
{
    constexpr auto sq = [](int x) { return x * x; };
    float const r = static_cast<float>(m_renderRadius) + 0.5f;
    float const r2 = r * r;

    std::vector<Magnum::Math::Vector3<int>> positions;
    positions.reserve((2 * m_renderRadius + 1) * (2 * m_renderRadius + 1));

    for (int dz = -m_renderRadius; dz <= m_renderRadius; ++dz)
    {
        for (int dx = -m_renderRadius; dx <= m_renderRadius; ++dx)
        {
            if (sq(dx) + sq(dz) > r2) continue;
            positions.emplace_back(currentChunkPos.x() + dx, 0, currentChunkPos.z() + dz);
        }
    }

    std::ranges::sort(positions, [&](auto const& a, auto const& b) {
        int da = sq(a.x() - currentChunkPos.x()) + sq(a.z() - currentChunkPos.z());
        int db = sq(b.x() - currentChunkPos.x()) + sq(b.z() - currentChunkPos.z());
        return da < db;
    });

    for (auto const& pos : positions)
    {
        auto it = m_chunkToMesh.find(pos);
        if (it != m_chunkToMesh.end())
        {
            for (Entity e : it->second)
            {
                if (auto mc = m_ecs.getComponent<MeshComponent>(e))
                {
                    m_shaderProgram
                        .bindTexture(m_textureManager->get(mc->textureName))
                        .draw(*mc->mesh);
                }
            }
        }
        else
        {
            enqueueChunkForMesh(pos);
        }
    }
}

void RenderSystem::enqueueChunkForMesh(Magnum::Math::Vector3<int> const& pos)
{
    if (m_enqueuedChunks.insert(pos).second)
    {
        SPAM_LOG(DEBUG, "Enqueue mesh for chunk [{}, {}]", pos.x(), pos.z());
        m_meshQueue.push(pos);
    }
}

size_t RenderSystem::processMeshQueue(time_point const& start)
{
    size_t launches = 0;
    while (!m_meshQueue.empty())
    {
        if (std::chrono::duration<double>(clock::now() - start).count() >= m_timeBudget)
            break;

        auto pos = m_meshQueue.front();
        m_meshQueue.pop();

        if (auto opt = m_world.getChunk(pos))
        {
            auto blocksMeshes = render::ChunkMeshBuilder::build(opt->get());
            auto& vec = m_chunkToMesh[pos];
            for (auto& mesh : blocksMeshes)
            {
                Entity e = m_ecs.createEntity();
                m_ecs.addComponent<MeshComponent>(e, mesh);
                vec.push_back(e);
            }
            SPAM_LOG(DEBUG, "Built {} sub-meshes for chunk [{}, {}]", blocksMeshes.size(), pos.x(), pos.z());
        }
        else
        {
            m_meshQueue.push(pos);
        }
        ++launches;
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
} // namespace mc::ecs
