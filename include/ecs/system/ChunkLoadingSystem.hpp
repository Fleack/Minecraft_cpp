#pragma once

#include "ecs/system/ISystem.hpp"

#include <glm/glm.hpp>

namespace mc::world
{
class World;
}

namespace mc::ecs
{
class ECS;

class ChunkLoadingSystem final : public ISystem
{
public:
    ChunkLoadingSystem(ECS& ecs, world::World& world, int radius = 2);
    void update(float deltaTime) override;

private:
    glm::ivec3 worldPosToChunk(const glm::vec3& pos) const;

private:
    ECS& m_ecs;
    world::World& m_world;
    int m_loadRadius{3};
    glm::ivec3 m_lastCameraChunk{999999};
};
}
