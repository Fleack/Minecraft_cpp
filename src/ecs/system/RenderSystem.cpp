#include "ecs/system/RenderSystem.hpp"

#include <Magnum/GL/Renderer.h>
#include <algorithm>

#include "core/Logger.hpp"
#include "ecs/Ecs.hpp"
#include "ecs/component/MeshComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/system/CameraSystem.hpp"
#include "render/ChunkMeshBuilder.hpp"
#include "world/Chunk.hpp"
#include "world/World.hpp"

#include <Magnum/GL/Shader.h>
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
    LOG(INFO, "RenderSystem initialized with render radius: {}", renderRadius);
}

void RenderSystem::update(float dt)
{
    constexpr float targetFrame = 1.0f / 60.0f;
    float leftover = targetFrame - dt;
    m_timeBudget = (leftover > 0.0f ? leftover : 0.0f) * workFraction;

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
        .setModelMatrix(Magnum::Matrix4{1.0f});

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
    Corrade::Utility::Debug{} << "Current camera chunk:" << currentChunkPos
                              << " total meshes:" << m_chunkToMesh.size();
    float radius = static_cast<float>(m_renderRadius) + 0.5f;
    float radiusSq = radius * radius;

    std::vector<Magnum::Math::Vector3<int>> candidates;
    candidates.reserve((2 * m_renderRadius + 1) * (2 * m_renderRadius + 1));
    for (int x = -m_renderRadius; x <= m_renderRadius; ++x)
    {
        for (int z = -m_renderRadius; z <= m_renderRadius; ++z)
        {
            if (x * x + z * z > radiusSq) continue;

            Magnum::Math::Vector3<int> pos = currentChunkPos + Magnum::Math::Vector3<int>{x, 0, z};
            auto it = m_chunkToMesh.find(pos);
            if (it != m_chunkToMesh.end())
            {
                if (auto comp = m_ecs.getComponent<MeshComponent>(it->second))
                {
                    m_shaderProgram.draw(*comp->mesh);
                }
            }
            else
            {
                candidates.push_back(pos);
            }
        }
    }

    std::ranges::sort(candidates, [&](auto const& a, auto const& b) {
        auto const da = a - currentChunkPos;
        auto const db = b - currentChunkPos;
        return da.x() * da.x() + da.z() * da.z() < db.x() * db.x() + db.z() * db.z();
    });

    for (auto const& pos : candidates)
    {
        enqueueChunkForMesh(pos);
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

        Magnum::Math::Vector3<int> pos = m_meshQueue.front();
        m_meshQueue.pop();

        if (auto opt = m_world.getChunk(pos))
        {
            auto mesh = std::make_shared<Magnum::GL::Mesh>();
            render::ChunkMeshBuilder::build(opt->get(), *mesh, 1.0f / 16.0f);
            Entity e = m_ecs.createEntity();
            m_ecs.addComponent<MeshComponent>(e, MeshComponent{mesh});
            m_chunkToMesh[pos] = e;
            SPAM_LOG(DEBUG, "Created mesh for chunk [{}, {}]", pos.x(), pos.z());
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
