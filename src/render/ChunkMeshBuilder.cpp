#include "render/ChunkMeshBuilder.hpp"

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
    if (x < 0 || x >= ChunkSizeX ||
        y < 0 || y >= ChunkSizeY ||
        z < 0 || z >= ChunkSizeZ)
    {
        return true;
    }
    return !chunk.getBlock(x, y, z).isSolid();
}
}

void ChunkMeshBuilder::build(world::Chunk const& chunk, ChunkMesh& mesh)
{
    std::vector<Vertex> vertices;

    for (int x = 0; x < world::ChunkSizeX; ++x)
    {
        for (int y = 0; y < world::ChunkSizeY; ++y)
        {
            for (int z = 0; z < world::ChunkSizeZ; ++z)
            {
                const auto block = chunk.getBlock(x, y, z);
                if (!block.isSolid()) { continue; }

                for (int face = 0; face < 6; ++face)
                {
                    const int nx = x + static_cast<int>(faceNormals[face].x);
                    const int ny = y + static_cast<int>(faceNormals[face].y);
                    const int nz = z + static_cast<int>(faceNormals[face].z);

                    if (!isFaceVisible(chunk, nx, ny, nz)) { continue; }

                    for (int i : {0, 1, 2, 0, 2, 3})
                    {
                        Vertex v;
                        v.position = glm::vec3(x, y, z) + faceVertices[face][i];
                        v.normal = faceNormals[face];
                        v.uv = uvs[i % 4];
                        vertices.push_back(v);
                    }
                }
            }
        }
    }

    mesh.upload(vertices);
}
}
