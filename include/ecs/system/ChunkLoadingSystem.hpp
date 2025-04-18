#pragma once

#include "ecs/system/ISystem.hpp"

#include <glm/glm.hpp>

namespace mc::world
{
class World;
}

namespace mc::ecs
{
class Ecs;

/**
 * @brief ECS system responsible for dynamic chunk loading around the player.
 *
 * Tracks the player's position via their transform component and ensures
 * that all chunks within a configurable radius are loaded as the player moves.
 */
class ChunkLoadingSystem final : public ISystem
{
public:
    /**
     * @brief Constructs the chunk loading system.
     *
     * @param ecs Reference to the ECS instance.
     * @param world Reference to the world containing all chunks.
     * @param radius Load radius (in chunks) around the active entity.
     */
    ChunkLoadingSystem(Ecs& ecs, world::World& world, uint8_t radius);

    /**
     * @brief Updates the system (called every frame).
     *
     * Converts the entity's world position to a chunk-space position,
     * and if it changed since the last frame, loads all surrounding chunks
     * within the defined radius.
     *
     * @param dt Delta time (not used currently).
     */
    void update(float dt) override;

    /**
     * @brief Does nothing.
     */
    void render() override {}

private:
    /**
     * @brief Converts a world-space position to chunk-space coordinates.
     *
     * @param pos World-space position (e.g., camera or player position).
     * @return Corresponding chunk-space coordinate.
     */
    glm::ivec3 worldPosToChunk(const glm::vec3& pos) const;

private:
    Ecs& m_ecs; ///< Reference to the ECS manager.
    world::World& m_world; ///< Reference to the ECS manager.
    uint8_t m_loadRadius; ///< Number of chunks to load around the current chunk.
    glm::ivec3 m_lastCameraChunk{std::numeric_limits<int>::max()}; ///< Tracks last known camera chunk position to avoid redundant loading.
};
} // namespace mc::ecs
