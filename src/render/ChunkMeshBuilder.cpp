#include "render/ChunkMeshBuilder.hpp"

#include "core/Logger.hpp"

#include "render/BlockTextureMapper.hpp"
#include "render/TextureAtlas.hpp"

namespace mc::render
{
namespace
{
/**
 * @brief Normal vectors for each of the six block faces.
 *
 * Order: front, back, top, bottom, right, left.
 */
constexpr glm::vec3 FACE_NORMALS[] = {
    {0, 0, 1}, // front
    {0, 0, -1}, // back
    {0, 1, 0}, // top
    {0, -1, 0}, // bottom
    {1, 0, 0}, // right
    {-1, 0, 0} // left
};

/**
 * @brief Vertex positions for each face of a unit cube.
 *
 * Each face has 4 vertices ordered counter-clockwise.
 * Indexed as [face][vertexIndex].
 */
constexpr glm::vec3 FACE_VERTICES[6][4] = {
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

/**
 * @brief UV coordinates for a quad (in normalized [0,1] space).
 *
 * Order: bottom-left, bottom-right, top-right, top-left.
 */
constexpr glm::vec2 UVS[] = {
    {0.0f, 0.0f}, {1.0f, 0.0f},
    {1.0f, 1.0f}, {0.0f, 1.0f}
};

/**
 * @brief Determines if a block face is visible.
 *
 * A face is considered visible if the neighboring block is out of bounds
 * or not solid.
 *
 * @param chunk The chunk to test against.
 * @param x X-coordinate of the neighbor block.
 * @param y Y-coordinate of the neighbor block.
 * @param z Z-coordinate of the neighbor block.
 * @return True if the face should be rendered, false otherwise.
 */
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

void ChunkMeshBuilder::build(world::Chunk const& chunk, ChunkMesh& mesh, TextureAtlas const& atlas)
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

                for (int face = 0; face < 6; ++face)
                {
                    const int nx = x + static_cast<int>(FACE_NORMALS[face].x);
                    const int ny = y + static_cast<int>(FACE_NORMALS[face].y);
                    const int nz = z + static_cast<int>(FACE_NORMALS[face].z);

                    if (!isFaceVisible(chunk, nx, ny, nz)) { continue; }

                    for (int i : {0, 1, 2, 0, 2, 3})
                    {
                        Vertex v;
                        v.position = glm::vec3(x, y, z) + FACE_VERTICES[face][i] + chunkOffset;
                        v.normal = FACE_NORMALS[face];

                        auto texName = getTextureNameForBlock(block.type);
                        auto tileUV = atlas.getUv(texName);
                        float tileSize = 1.0f / static_cast<float>(atlas.getAtlasSize());

                        v.uv = tileUV + (UVS[i % 4] * tileSize);
                        vertices.push_back(v);
                    }
                }
            }
        }
    }

    mesh.upload(vertices);
}
}
