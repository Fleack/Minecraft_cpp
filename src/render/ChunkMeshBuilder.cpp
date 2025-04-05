#include "render/ChunkMeshBuilder.hpp"

#include <random>

static std::mt19937 g_rng(std::random_device{}());
static std::uniform_real_distribution<float> g_colorDist(0.0f, 1.0f);

namespace mc::render
{
namespace
{
const glm::vec3 faceNormals[] = {
    {0, 0, 1}, // front
    {0, 0, -1}, // back
    {0, 1, 0}, // top
    {0, -1, 0}, // bottom
    {1, 0, 0}, // right
    {-1, 0, 0} // left
};

const glm::vec3 faceVertices[6][4] = {
    // Front
    {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}},
    // Back
    {{1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {1, 1, 0}},
    // Top
    {{0, 1, 1}, {1, 1, 1}, {1, 1, 0}, {0, 1, 0}},
    // Bottom
    {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}},
    // Right
    {{1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 1}},
    // Left
    {{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0}}
};

constexpr glm::vec2 uvs[] = {
    {0.0f, 0.0f}, {1.0f, 0.0f},
    {1.0f, 1.0f}, {0.0f, 1.0f}
};

bool isFaceVisible(world::Chunk const& chunk, int x, int y, int z)
{
    using namespace world;
    if (x < 0 || x >= CHUNK_SIZE_X ||
        y < 0 || y >= CHUNK_SIZE_Y ||
        z < 0 || z >= CHUNK_SIZE_Z)
    {
        return true;
    }
    return !chunk.getBlock(x, y, z).isSolid();
}
}

void ChunkMeshBuilder::build(world::Chunk const& chunk, ChunkMesh& mesh)
{
    mesh.setChunkPosition(chunk.getPosition());

    auto chunkOffset = glm::vec3(
        chunk.getPosition().x * world::CHUNK_SIZE_X,
        chunk.getPosition().y * world::CHUNK_SIZE_Y,
        chunk.getPosition().z * world::CHUNK_SIZE_Z
    );

    std::vector<Vertex> vertices;

    for (int x = 0; x < world::CHUNK_SIZE_X; ++x)
    {
        for (int y = 0; y < world::CHUNK_SIZE_Y; ++y)
        {
            for (int z = 0; z < world::CHUNK_SIZE_Z; ++z)
            {
                const auto block = chunk.getBlock(x, y, z);
                if (!block.isSolid()) { continue; }

                glm::vec3 blockColor = {
                    g_colorDist(g_rng),
                    g_colorDist(g_rng),
                    g_colorDist(g_rng)
                };

                for (int face = 0; face < 6; ++face)
                {
                    const int nx = x + static_cast<int>(faceNormals[face].x);
                    const int ny = y + static_cast<int>(faceNormals[face].y);
                    const int nz = z + static_cast<int>(faceNormals[face].z);

                    if (!isFaceVisible(chunk, nx, ny, nz)) { continue; }

                    for (int i : {0, 1, 2, 0, 2, 3})
                    {
                        Vertex v;
                        v.position = glm::vec3(x, y, z) + faceVertices[face][i] + chunkOffset;
                        v.normal = faceNormals[face];
                        v.uv = uvs[i % 4];
                        v.color = blockColor;
                        vertices.push_back(v);
                    }
                }
            }
        }
    }

    mesh.upload(vertices);
}
}
