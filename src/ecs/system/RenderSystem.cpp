#include <glad/gl.h> // Must be first

#include "ecs/system/RenderSystem.hpp"

#include "ecs/component/MeshComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/system/CameraSystem.hpp"
#include "render/ChunkMesh.hpp"
#include "render/Shader.hpp"
#include "world/Chunk.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "render/ChunkMeshBuilder.hpp"

#include "world/World.hpp"

namespace mc::ecs
{
RenderSystem::RenderSystem(ECS& ecs,
                           std::shared_ptr<CameraSystem> cameraSystem,
                           world::World& world,
                           uint8_t renderRadius)
    : m_ecs{ecs}, m_cameraSystem{std::move(cameraSystem)}, m_world{world}, m_renderRadius{renderRadius}
{
    m_shader = std::make_shared<render::Shader>("shaders/voxel.vert", "shaders/voxel.frag");
}

void RenderSystem::update(float /*deltaTime*/)
{
    m_shader->bind();

    const glm::mat4 view = m_cameraSystem->getViewMatrix();
    const glm::mat4 projection = m_cameraSystem->getProjectionMatrix();

    glUniformMatrix4fv(glGetUniformLocation(m_shader->getID(), "u_View"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_shader->getID(), "u_Projection"), 1, GL_FALSE, glm::value_ptr(projection));

    const auto& cameraTransform = m_ecs.getAllComponents<TransformComponent>().begin()->second;
    const glm::vec3 cameraPos = cameraTransform.position;

    const glm::ivec3 currentChunkPos = {
        static_cast<int>(std::floor(cameraPos.x / static_cast<float>(world::CHUNK_SIZE_X))),
        0,
        static_cast<int>(std::floor(cameraPos.z / static_cast<float>(world::CHUNK_SIZE_Z)))
    };

    drawChunksInRadius(currentChunkPos);

    m_shader->unbind();
}

void RenderSystem::drawChunksInRadius(const glm::ivec3& currentChunkPos)
{
    for (int x = -m_renderRadius; x <= m_renderRadius; ++x)
    {
        for (int z = -m_renderRadius; z <= m_renderRadius; ++z)
        {
            glm::ivec3 chunkPos = currentChunkPos + glm::ivec3{x, 0, z};

            if (!m_chunkToEntity.contains(chunkPos))
            {
                auto mesh = std::make_shared<render::ChunkMesh>();
                render::ChunkMeshBuilder::build(m_world.getChunk(chunkPos).value(), *mesh);

                auto entity = m_ecs.createEntity();
                m_ecs.addComponent<MeshComponent>(entity, MeshComponent{mesh});

                m_chunkToEntity[chunkPos] = entity;
            }

            auto it = m_chunkToEntity.find(chunkPos);
            auto meshComp = m_ecs.getComponent<MeshComponent>(it->second);
            if (!meshComp || !meshComp->mesh) { continue; }

            auto model = glm::mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(m_shader->getID(), "u_Model"), 1, GL_FALSE, glm::value_ptr(model));

            meshComp->mesh->draw();
        }
    }
}
}
