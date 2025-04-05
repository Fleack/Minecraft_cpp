#pragma once

#include <vector>
#include <glad/gl.h>
#include <glm/glm.hpp>

namespace mc::render
{
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 color;
};

class ChunkMesh
{
public:
    ChunkMesh();
    ~ChunkMesh();

    void upload(std::vector<Vertex> const& vertices);
    void draw() const;

    void setChunkPosition(glm::ivec3 pos) { m_chunkPosition = pos; }
    [[nodiscard]] glm::ivec3 getChunkPosition() const { return m_chunkPosition; }

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    std::size_t m_vertexCount = 0;
    glm::ivec3 m_chunkPosition{0, 0, 0};
};
}
