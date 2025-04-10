#pragma once

#include <vector>
#include <glad/gl.h>
#include <glm/glm.hpp>

namespace mc::render
{
/**
 * @brief Represents a single vertex in a mesh.
 *
 * Contains position, normal, and texture coordinates.
 */
struct Vertex
{
    glm::vec3 position; ///< Vertex position in world space.
    glm::vec3 normal; ///< Normal vector for lighting.
    glm::vec2 uv; ///< Texture coordinates.
};

/**
 * @brief GPU mesh representation for a voxel chunk.
 *
 * Manages OpenGL buffers and rendering of chunk geometry.
 */
class ChunkMesh
{
public:
    /**
     * @brief Constructs an empty chunk mesh.
     */
    ChunkMesh(); // TODO: chunk position
    ~ChunkMesh() = default;

    /**
     * @brief Uploads vertex data to the GPU.
     *
     * @param vertices List of vertices to upload.
     */
    void upload(std::vector<Vertex> const& vertices);

    /**
     * @brief Draws the mesh using OpenGL.
     */
    void draw() const;

    void setChunkPosition(glm::ivec3 pos) { m_chunkPosition = pos; }
    [[nodiscard]] glm::ivec3 getChunkPosition() const { return m_chunkPosition; }

private:
    GLuint m_vao = 0; ///< Vertex array object.
    GLuint m_vbo = 0; ///< Vertex buffer object.
    std::size_t m_vertexCount = 0; ///< Number of vertices in the mesh.
    glm::ivec3 m_chunkPosition{0, 0, 0}; ///< Position of the chunk in the world.
};
}
