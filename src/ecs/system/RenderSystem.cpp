#include "ecs/system/RenderSystem.hpp"

#include "ecs/component/MeshComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/system/CameraSystem.hpp"
#include "render/ChunkMesh.hpp"
#include "render/Shader.hpp"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

namespace mc::ecs
{
RenderSystem::RenderSystem(ECS& ecs, std::shared_ptr<CameraSystem> cameraSystem)
    : m_ecs(ecs), m_cameraSystem(std::move(cameraSystem))
{
    m_shader = std::make_shared<render::Shader>("shaders/voxel.vert", "shaders/voxel.frag");
}

void RenderSystem::update(float /*deltaTime*/)
{
    m_shader->bind();

    glm::mat4 view = m_cameraSystem->getViewMatrix();
    glm::mat4 projection = m_cameraSystem->getProjectionMatrix();

    glUniformMatrix4fv(glGetUniformLocation(m_shader->getID(), "u_View"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_shader->getID(), "u_Projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));

    auto& meshes = m_ecs.getAllComponents<MeshComponent>();

    for (auto& [entity, meshComp] : meshes)
    {
        auto model = glm::mat4(1.0f); // TODO TransformComponent add later
        glUniformMatrix4fv(glGetUniformLocation(m_shader->getID(), "u_Model"), 1, GL_FALSE, glm::value_ptr(model));

        if (meshComp.mesh)
            meshComp.mesh->draw();
    }

    m_shader->unbind();
}
}
