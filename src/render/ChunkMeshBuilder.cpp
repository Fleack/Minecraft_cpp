#include "render/ChunkMeshBuilder.hpp"

#include "ecs/component/MeshComponent.hpp"
#include "render/BlockTextureMapper.hpp"
#include "render/Vertex.hpp"

#include <memory>
#include <unordered_map>

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
constexpr Math::Vector2<Float> FACE_UVS[6][4] = {
    // Front
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}},
    // Back
    {{1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}},
    // Top
    {{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}},
    // Bottom
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}},
    // Right
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}},
    // Left
    {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}},
};

bool is_face_visible(world::Chunk const& chunk, int x, int y, int z)
{
    using namespace world;
    if (x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE_Z)
        return true;
    return not chunk.getBlock(x, y, z).isSolid();
}

} // namespace

std::vector<ecs::MeshComponent> ChunkMeshBuilder::build(world::Chunk const& chunk)
{
    using namespace world;
    std::unordered_map<std::string, std::vector<Vertex>> byTexture;

    auto const chunkOffset = Magnum::Math::Vector3<Int>{
        chunk.getPosition().x() * CHUNK_SIZE_X,
        chunk.getPosition().y() * CHUNK_SIZE_Y,
        chunk.getPosition().z() * CHUNK_SIZE_Z};

    for (int x = 0; x < CHUNK_SIZE_X; ++x)
        for (int y = 0; y < CHUNK_SIZE_Y; ++y)
            for (int z = 0; z < CHUNK_SIZE_Z; ++z)
            {
                auto const block = chunk.getBlock(x, y, z);
                if (!block.isSolid()) continue;
                for (uint8_t face = 0; face < 6; ++face)
                {
                    auto const nx = x + static_cast<int>(FACE_NORMALS[face].x());
                    auto const ny = y + static_cast<int>(FACE_NORMALS[face].y());
                    auto const nz = z + static_cast<int>(FACE_NORMALS[face].z());

                    if (!is_face_visible(chunk, nx, ny, nz)) continue;

                    auto textureName = get_texture_name_for_block(block.type, face);
                    for (int i : {0, 1, 2, 0, 2, 3})
                    {
                        Vertex v;
                        v.position = Vector3(x, y, z) + Vector3(FACE_VERTICES[face][i]) + Vector3(chunkOffset);
                        v.normal = Vector3(FACE_NORMALS[face]);
                        v.uv = FACE_UVS[face][i % 4];
                        byTexture[textureName].push_back(v);
                    }
                }
            }

    std::vector<ecs::MeshComponent> result;
    result.reserve(byTexture.size());
    for (auto& [texName, verts] : byTexture)
    {
        auto mesh = std::make_shared<GL::Mesh>();
        GL::Buffer buf;
        buf.setData(Containers::arrayView(verts.data(), verts.size()), GL::BufferUsage::StaticDraw);
        mesh->setPrimitive(GL::MeshPrimitive::Triangles)
            .setCount(verts.size())
            .addVertexBuffer(
                std::move(buf), 0, attribute::POSITION_ATTRIBUTE, attribute::NORMAL_ATTRIBUTE, attribute::UV_ATTRIBUTE);
        result.push_back({mesh, texName});
    }
    return result;
}

} // namespace mc::render
