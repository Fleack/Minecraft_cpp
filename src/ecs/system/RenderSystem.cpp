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

void RenderSystem::update(float /*deltaTime*/) {}

void RenderSystem::render()
{
    m_shader->bind();
    m_atlas->bind();

    glm::mat4 const view = m_cameraSystem->getViewMatrix();
    glm::mat4 const projection = m_cameraSystem->getProjectionMatrix();

    glUniform1i(glGetUniformLocation(m_shader->getId(), "u_Texture"), 0);
    glUniformMatrix4fv(glGetUniformLocation(m_shader->getId(), "u_View"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_shader->getId(), "u_Projection"), 1, GL_FALSE, glm::value_ptr(projection));

    auto const& cameraTransform = m_ecs.getAllComponents<TransformComponent>().begin()->second;
    glm::vec3 const cameraPos = cameraTransform.position;

    glm::ivec3 const currentChunkPos = {
        static_cast<int>(std::floor(cameraPos.x / static_cast<float>(world::CHUNK_SIZE_X))), 0, static_cast<int>(std::floor(cameraPos.z / static_cast<float>(world::CHUNK_SIZE_Z)))};

    drawChunksInRadius(currentChunkPos);

    m_shader->unbind();
}

void RenderSystem::drawChunksInRadius(glm::ivec3 const& currentChunkPos)
{
    float const radiusWithPadding = static_cast<float>(m_renderRadius) + 0.5f;
    float const radiusSq = radiusWithPadding * radiusWithPadding;
    for (int x = -m_renderRadius; x <= m_renderRadius; ++x)
    {
        for (int z = -m_renderRadius; z <= m_renderRadius; ++z)
        {
            if (static_cast<float>(x * x + z * z) > radiusSq) continue;
            glm::ivec3 chunkPos = currentChunkPos + glm::ivec3{x, 0, z};

            if (!m_chunkToMesh.contains(chunkPos))
            {
                auto chunk = m_world.getChunk(chunkPos);
                if (!chunk)
                {
                    LOG(DEBUG, "Chunk at position [{}, {}] not found", chunkPos.x, chunkPos.z);
                    continue;
                }
                LOG(INFO, "Creating mesh for chunk at [{}, {}]", chunkPos.x, chunkPos.z);
                auto mesh = std::make_shared<render::ChunkMesh>(chunkPos);
                render::ChunkMeshBuilder::build(*chunk, *mesh, *m_atlas.get());

                auto entity = m_ecs.createEntity();
                m_ecs.addComponent<MeshComponent>(entity, MeshComponent{mesh});

                m_chunkToMesh[chunkPos] = entity;
            }

            auto it = m_chunkToMesh.find(chunkPos);
            if (it == m_chunkToMesh.end())
            {
                LOG(CRITICAL, "Missing entity for chunk at [{}, {}], seems like a bug", chunkPos.x, chunkPos.z);
                continue;
            }

            auto meshComp = m_ecs.getComponent<MeshComponent>(it->second);
            if (!meshComp || !meshComp->mesh)
            {
                LOG(WARN, "Missing or invalid mesh for chunk at [{}, {}]", chunkPos.x, chunkPos.z);
                continue;
            }

            auto model = glm::mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(m_shader->getId(), "u_Model"), 1, GL_FALSE, glm::value_ptr(model));

            meshComp->mesh->draw();
        }
    }
}
} // namespace mc::ecs
