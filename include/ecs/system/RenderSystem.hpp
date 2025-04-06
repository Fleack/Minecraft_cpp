#pragma once

#include "ecs/system/ISystem.hpp"
#include "utils/IVer3Hasher.hpp"

#include <memory>

#include "ecs/Entity.hpp"
#include "render/TextureAtlas.hpp"

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
    RenderSystem(ECS& ecs,
                 std::shared_ptr<CameraSystem> cameraSystem,
                 std::unique_ptr<render::IShader> shader,
                 std::unique_ptr<render::TextureAtlas> atlas,
                 world::World& world,
                 uint8_t renderRadius);

    void update(float deltaTime) override;

private:
    void drawChunksInRadius(glm::ivec3 const& currentChunkPos);

private:
    ECS& m_ecs;
    world::World& m_world;

    std::shared_ptr<CameraSystem> m_cameraSystem;
    std::unique_ptr<render::IShader> m_shader;
    std::unique_ptr<render::TextureAtlas> m_atlas;

    std::unordered_map<glm::ivec3, Entity, utils::IVec3Hasher> m_chunkToEntity;

    uint8_t m_renderRadius {5};
};
}
