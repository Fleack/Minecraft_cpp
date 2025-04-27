#include <glad/gl.h> // Must be first

#include "ecs/system/RenderSystem.hpp"

#include "core/Logger.hpp"
#include "ecs/component/MeshComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/system/CameraSystem.hpp"
#include "render/ChunkMesh.hpp"
#include "render/ChunkMeshBuilder.hpp"
#include "render/Shader.hpp"
#include "world/Chunk.hpp"
#include "world/World.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace mc::ecs
{
RenderSystem::RenderSystem(
    Ecs& ecs,
    std::shared_ptr<CameraSystem> cameraSystem,
    std::unique_ptr<render::IShader> shader,
    std::unique_ptr<render::TextureAtlas> atlas,
    world::World& world,
    uint8_t renderRadius)
    : m_ecs{ecs}, m_world{world}, m_cameraSystem{std::move(cameraSystem)}, m_shader{std::move(shader)}, m_atlas{std::move(atlas)}, m_renderRadius{renderRadius}
{
    m_atlas->loadFromDirectory("assets/textures/blocks/");
    LOG(INFO, "RenderSystem initialized with render radius: {}", renderRadius);
}

void RenderSystem::update(float dt)
{
    constexpr float targetFrame = 1.0f / 60.0f;
    float leftover = targetFrame - dt;
    m_timeBudget = (leftover > 0.0f ? leftover : 0.0f) * workFraction;

    auto opt = getCurrentChunk();
    if (!opt) return;

    static glm::ivec3 lastChunk{INT_MIN, 0, INT_MIN};
    if (*opt != lastChunk)
    {
        lastChunk = *opt;
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
    m_shader->bind();
    m_atlas->bind();

    glm::mat4 const view = m_cameraSystem->getViewMatrix();
    glm::mat4 const projection = m_cameraSystem->getProjectionMatrix();
    glUniform1i(glGetUniformLocation(m_shader->getId(), "u_Texture"), 0);
    glUniformMatrix4fv(glGetUniformLocation(m_shader->getId(), "u_View"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_shader->getId(), "u_Projection"), 1, GL_FALSE, glm::value_ptr(projection));

    if (auto opt = getCurrentChunk())
    {
        drawChunksInRadius(*opt);
    }

    m_shader->unbind();
}

std::optional<glm::ivec3> RenderSystem::getCurrentChunk() const
{
    auto& transforms = m_ecs.getAllComponents<TransformComponent>();
    if (transforms.empty()) return std::nullopt;
    auto const& t = transforms.begin()->second.position;

    return glm::ivec3{
        static_cast<int>(std::floor(t.x / static_cast<float>(world::CHUNK_SIZE_X))),
        0,
        static_cast<int>(std::floor(t.z / static_cast<float>(world::CHUNK_SIZE_Z)))};
}

void RenderSystem::drawChunksInRadius(glm::ivec3 const& currentChunkPos)
{
    float const generateRadius = m_renderRadius + 0.5f;
    float const squaredGenerateRadius = generateRadius * generateRadius;

    for (int x = -m_renderRadius; x <= m_renderRadius; ++x)
    {
        for (int z = -m_renderRadius; z <= m_renderRadius; ++z)
        {
            if (static_cast<float>(x * x + z * z) > squaredGenerateRadius) continue;
            glm::ivec3 pos = currentChunkPos + glm::ivec3{x, 0, z};

            if (!m_chunkToMesh.contains(pos))
            {
                if (m_enqueuedChunks.insert(pos).second)
                {
                    m_meshQueue.push(pos);
                }
                continue;
            }

            Entity e = m_chunkToMesh[pos];
            if (auto comp = m_ecs.getComponent<MeshComponent>(e))
            {
                glm::mat4 model = glm::mat4(1.0f);
                glUniformMatrix4fv(
                    glGetUniformLocation(m_shader->getId(), "u_Model"),
                    1,
                    GL_FALSE,
                    glm::value_ptr(model));
                comp->mesh->draw();
            }
        }
    }
}

void RenderSystem::enqueueChunkForMesh(glm::ivec3 const& chunkPos)
{
    if (m_enqueuedChunks.insert(chunkPos).second)
    {
        m_meshQueue.push(chunkPos);
    }
}

size_t RenderSystem::processMeshQueue(time_point const& start)
{
    size_t launches = 0;
    while (!m_meshQueue.empty())
    {
        auto now = clock::now();
        if (std::chrono::duration<double>(now - start).count() >= m_timeBudget)
            break;

        glm::ivec3 pos = m_meshQueue.front();
        m_meshQueue.pop();

        if (auto opt = m_world.getChunk(pos))
        {
            LOG(DEBUG, "Building mesh for chunk [{}, {}]", pos.x, pos.z);
            auto mesh = std::make_shared<render::ChunkMesh>(pos);
            render::ChunkMeshBuilder::build(opt->get(), *mesh, *m_atlas);

            Entity e = m_ecs.createEntity();
            m_ecs.addComponent<MeshComponent>(e, MeshComponent{mesh});
            m_chunkToMesh[pos] = e;
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
    auto end = clock::now();
    double total = std::chrono::duration<double>(end - start).count();
    double avg = total / static_cast<double>(launches);

    m_avgBuildTime = alpha * avg + (1.0 - alpha) * m_avgBuildTime;
    LOG(DEBUG,
        "Built {} meshes in {:.3f} ms (EMA {:.3f} ms)",
        launches,
        total * 1000.0,
        m_avgBuildTime * 1000.0);
}
} // namespace mc::ecs
