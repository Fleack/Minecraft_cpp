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
class Ecs;
}

namespace mc::render
{
class IShader;
}

namespace mc::ecs
{
class CameraSystem;

/**
 * @brief ECS system responsible for rendering voxel chunks.
 *
 * Uses a camera system and a chunk mesh cache to render visible chunks
 * within a configurable radius around the camera. Handles shader binding,
 * view/projection setup, and draws each chunk mesh with the proper model transform.
 */
class RenderSystem final : public ISystem
{
public:
    /**
     * @brief Constructs the rendering system.
     *
     * @param ecs Reference to the ECS registry.
     * @param cameraSystem Shared camera system to retrieve view/projection matrices.
     * @param shader Shader program used for rendering chunk geometry.
     * @param atlas Texture atlas containing block textures.
     * @param world Reference to the game world.
     * @param renderRadius Number of chunks to render in each direction from the camera.
     */
    RenderSystem(Ecs& ecs,
                 std::shared_ptr<CameraSystem> cameraSystem,
                 std::unique_ptr<render::IShader> shader,
                 std::unique_ptr<render::TextureAtlas> atlas,
                 world::World& world,
                 uint8_t renderRadius);

    /**
     * @brief Does nothing.
     */
    void update(float deltaTime) override;

    /**
     * @brief Renders visible chunk meshes using the active camera.
     *
     * Binds the shader, sets uniforms, and draws all chunk meshes
     * within the configured render radius.
     */
    void render() override;

private:
    /**
     * @brief Draws all chunks within the render radius of the camera.
     *
     * Builds chunk meshes on demand and caches them via ECS entities.
     *
     * @param currentChunkPos Position of the camera in chunk space.
     */
    void drawChunksInRadius(glm::ivec3 const& currentChunkPos);

private:
    Ecs& m_ecs; ///< ECS manager reference.
    world::World& m_world; ///< Reference to the world for chunk access.

    std::shared_ptr<CameraSystem> m_cameraSystem; ///< Provides view and projection matrices.
    std::unique_ptr<render::IShader> m_shader; ///< Shader program used for rendering.
    std::unique_ptr<render::TextureAtlas> m_atlas; ///< Texture atlas for block textures.

    uint8_t m_renderRadius; ///< Radius (in chunks) for rendering around the camera.

    std::unordered_map<glm::ivec3, Entity, utils::IVec3Hasher> m_chunkToEntity; ///< Maps chunk positions to ECS entities holding their mesh.
};
}
