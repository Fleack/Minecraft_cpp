#include "ecs/system/ChunkLoadingSystem.hpp"

#include "core/Logger.hpp"
#include "ecs/Ecs.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "world/World.hpp"

namespace mc::ecs
{
ChunkLoadingSystem::ChunkLoadingSystem(Ecs& ecs, world::World& world, uint8_t radius)
    : m_ecs(ecs), m_world(world), m_loadRadius(radius)
{
    LOG(INFO, "ChunkLoadingSystem initialized with load radius: {}", radius);
}

glm::ivec3 ChunkLoadingSystem::worldPosToChunk(glm::vec3 const& pos) const
{
    using namespace mc::world;
    return {
        static_cast<int>(std::floor(pos.x / static_cast<float>(CHUNK_SIZE_X))),
        0,
        static_cast<int>(std::floor(pos.z / static_cast<float>(CHUNK_SIZE_Z)))
    };
}

void ChunkLoadingSystem::update(float /*dt*/)
{
    auto& transforms = m_ecs.getAllComponents<TransformComponent>();
    if (transforms.empty()) return;

    const auto& transform = transforms.begin()->second;
    const glm::ivec3 currentChunk = worldPosToChunk(transform.position);

    if (currentChunk == m_lastCameraChunk) return;
    m_lastCameraChunk = currentChunk;
    LOG(INFO, "Camera moved to new chunk at position [{}, {}]",
        currentChunk.x, currentChunk.z);

    const float radiusWithPadding = static_cast<float>(m_loadRadius) + 0.5f;
    const float radiusSq = radiusWithPadding * radiusWithPadding;
    for (int x = -m_loadRadius; x <= m_loadRadius; ++x)
    {
        for (int z = -m_loadRadius; z <= m_loadRadius; ++z)
        {
            if (static_cast<float>(x * x + z * z) > radiusSq) continue;

            glm::ivec3 chunkPos = currentChunk + glm::ivec3{x, 0, z};
            LOG(DEBUG, "Loading chunk at position [{}, {}]", chunkPos.x, chunkPos.z);
            m_world.loadChunk(chunkPos);
        }
    }
}
}
