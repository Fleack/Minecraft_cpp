#pragma once

#include "ecs/system/ISystem.hpp"
#include "utils/IVer3Hasher.hpp"

#include <memory>

#include "ecs/Entity.hpp"

namespace mc::world
{
class World;
}

namespace mc::ecs
{
class ECS;
}

namespace mc::render
{
class IShader;
}

namespace mc::ecs
{
class CameraSystem;

class RenderSystem final : public ISystem
{
public:
    RenderSystem(ECS& ecs, std::shared_ptr<CameraSystem> cameraSystem, std::shared_ptr<render::IShader> shader, world::World& world, uint8_t renderRadius);

    void update(float deltaTime) override;

private:
    void drawChunksInRadius(glm::ivec3 const& currentChunkPos);

private:
    ECS& m_ecs;
    std::shared_ptr<CameraSystem> m_cameraSystem;
    std::shared_ptr<render::IShader> m_shader;
    world::World& m_world;

    std::unordered_map<glm::ivec3, Entity, utils::IVec3Hasher> m_chunkToEntity;

    uint8_t m_renderRadius {5};
};
}
