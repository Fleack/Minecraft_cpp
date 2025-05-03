#include "render/ChunkMeshBuilder.hpp"
#include "render/Vertex.hpp"

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Vector3.h>

namespace mc::render
{
using namespace Magnum;

namespace
{
/**
 * @brief Normal vectors for each of the six block faces.
 *
 * Order: front, back, top, bottom, right, left.
 */
constexpr Math::Vector3<Int> FACE_NORMALS[] = {
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
constexpr Math::Vector3<Int> FACE_VERTICES[6][4] = {
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
    {{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0}}};

/**
 * @brief Vertex order (indexes in UVS) for each face (6 faces with 6 triangular indexes)
 */
constexpr Math::Vector2<Int> FACE_UVS[6][4] = {
    // Front
    {{0, 1}, {1, 1}, {1, 0}, {0, 0}},
    // Back
    {{1, 1}, {0, 1}, {0, 0}, {1, 0}},
    // Top
    {{0, 0}, {1, 0}, {1, 1}, {0, 1}},
    // Bottom
    {{0, 1}, {1, 1}, {1, 0}, {0, 0}},
    // Right
    {{0, 1}, {1, 1}, {1, 0}, {0, 0}},
    // Left
    {{1, 1}, {0, 1}, {0, 0}, {1, 0}}};

bool is_face_visible(world::Chunk const& chunk, int x, int y, int z)
{
    using namespace world;
    if (x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE_Z)
        return true;
    return !chunk.getBlock(x, y, z).isSolid();
}

} // namespace

void ChunkMeshBuilder::build(world::Chunk const& chunk, GL::Mesh& mesh, float uvTileSize)
{
    using namespace world;

    auto const chunkOffset = Magnum::Math::Vector3<Int>{
        chunk.getPosition().x() * CHUNK_SIZE_X,
        chunk.getPosition().y() * CHUNK_SIZE_Y,
        chunk.getPosition().z() * CHUNK_SIZE_Z};

    std::vector<Vertex> vertices;
    // vertices.reserve(CHUNK_VOLUME * averageFaces * 6);
    for (int x = 0; x < CHUNK_SIZE_X; ++x)
    {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y)
        {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z)
            {
                auto const block = chunk.getBlock(x, y, z);
                if (!block.isSolid()) continue;

                for (int face = 0; face < 6; ++face)
                {
                    auto const nx = x + static_cast<int>(FACE_NORMALS[face].x());
                    auto const ny = y + static_cast<int>(FACE_NORMALS[face].y());
                    auto const nz = z + static_cast<int>(FACE_NORMALS[face].z());

                    if (!is_face_visible(chunk, nx, ny, nz)) continue;

                    int const tileIndex = static_cast<int>(block.type);
                    int const atlasSize = static_cast<int>(1.0f / uvTileSize);
                    Math::Vector2<Float> const tileUv = {
                        static_cast<float>(tileIndex % atlasSize) * uvTileSize,
                        static_cast<float>(tileIndex / atlasSize) * uvTileSize};

                    for (int i : {0, 1, 2, 0, 2, 3})
                    {
                        Vertex v;
                        v.position = Magnum::Math::Vector3<Float>(x, y, z) + Magnum::Math::Vector3<Float>(FACE_VERTICES[face][i]) + Magnum::Math::Vector3<Float>(chunkOffset);
                        v.normal = Magnum::Math::Vector3<Float>(FACE_NORMALS[face]);
                        v.uv = tileUv + Math::Vector2<Float>(FACE_UVS[face][i % 4] * uvTileSize);
                        vertices.push_back(v);
                    }
                }
            }
        }
    }

    // Upload data to GPU
    GL::Buffer vertexBuffer;
    vertexBuffer.setData(Containers::ArrayView<Vertex const>{vertices.data(), vertices.size()}, GL::BufferUsage::StaticDraw);

    mesh.setCount(vertices.size())
        .setPrimitive(GL::MeshPrimitive::Triangles)
        .addVertexBuffer(
            std::move(vertexBuffer), 0, attribute::POSITION_ATTRIBUTE, attribute::NORMAL_ATTRIBUTE, attribute::UV_ATTRIBUTE);
}

} // namespace mc::render
