#include "render/ChunkMeshBuilder.hpp"

#include "ecs/component/MeshComponent.hpp"
#include "render/BlockTextureMapper.hpp"
#include "render/Vertex.hpp"
#include "world/World.hpp"

#include <memory>
#include <unordered_map>

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Functions.h>
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

constexpr std::tuple<Magnum::Math::Vector3<int>, Magnum::Math::Vector3<int>, Magnum::Math::Vector3<int>> AO_OFFSETS[6][4] = {
    // FRONT (+Z)
    {
        {{0, -1, 0}, {-1, 0, 0}, {-1, -1, 1}}, // Bottom Left
        {{0, -1, 0}, {1, 0, 0}, {1, -1, 1}}, // Bottom Right
        {{0, 1, 0}, {1, 0, 0}, {1, 1, 1}}, // Top Right
        {{0, 1, 0}, {-1, 0, 0}, {-1, 1, 1}}, // Top Left
    },
    // BACK (-Z)
    {
        {{0, -1, 0}, {1, 0, 0}, {1, -1, -1}},
        {{0, -1, 0}, {-1, 0, 0}, {-1, -1, -1}},
        {{0, 1, 0}, {-1, 0, 0}, {-1, 1, -1}},
        {{0, 1, 0}, {1, 0, 0}, {1, 1, -1}},
    },
    // TOP (+Y)
    {
        {{0, 0, 1}, {-1, 0, 0}, {-1, 0, 1}},
        {{0, 0, 1}, {1, 0, 0}, {1, 0, 1}},
        {{0, 0, -1}, {1, 0, 0}, {1, 0, -1}},
        {{0, 0, -1}, {-1, 0, 0}, {-1, 0, -1}},
    },
    // BOTTOM (-Y)
    {
        {{0, 0, -1}, {-1, 0, 0}, {-1, 0, -1}},
        {{0, 0, -1}, {1, 0, 0}, {1, 0, -1}},
        {{0, 0, 1}, {1, 0, 0}, {1, 0, 1}},
        {{0, 0, 1}, {-1, 0, 0}, {-1, 0, 1}},
    },
    // RIGHT (+X)
    {
        {{0, -1, 0}, {0, 0, 1}, {1, -1, 1}},
        {{0, -1, 0}, {0, 0, -1}, {1, -1, -1}},
        {{0, 1, 0}, {0, 0, -1}, {1, 1, -1}},
        {{0, 1, 0}, {0, 0, 1}, {1, 1, 1}},
    },
    // LEFT (-X)
    {
        {{0, -1, 0}, {0, 0, -1}, {-1, -1, -1}},
        {{0, -1, 0}, {0, 0, 1}, {-1, -1, 1}},
        {{0, 1, 0}, {0, 0, 1}, {-1, 1, 1}},
        {{0, 1, 0}, {0, 0, -1}, {-1, 1, -1}},
    },
};

bool get_block_solid(world::World const& world, Magnum::Math::Vector3<Int> const& pos)
{
    using namespace world;

    Magnum::Math::Vector3<Int> chunkPos{
        static_cast<int>(std::floor(pos.x() / static_cast<float>(CHUNK_SIZE_X))),
        static_cast<int>(std::floor(pos.y() / static_cast<float>(CHUNK_SIZE_Y))),
        static_cast<int>(std::floor(pos.z() / static_cast<float>(CHUNK_SIZE_Z)))};

    Magnum::Math::Vector3<Int> localPos{
        (pos.x() % CHUNK_SIZE_X + CHUNK_SIZE_X) % CHUNK_SIZE_X,
        (pos.y() % CHUNK_SIZE_Y + CHUNK_SIZE_Y) % CHUNK_SIZE_Y,
        (pos.z() % CHUNK_SIZE_Z + CHUNK_SIZE_Z) % CHUNK_SIZE_Z};

    if (auto chunkOpt = world.getChunk(chunkPos))
    {
        auto& chunk = chunkOpt->get();
        return chunk.getBlock(localPos.x(), localPos.y(), localPos.z()).isSolid();
    }

    return false;
}

} // namespace

std::vector<ecs::MeshComponent> ChunkMeshBuilder::build(world::Chunk const& chunk, world::World const& world)
{
    using namespace world;
    using namespace Magnum;
    using namespace Magnum::Math;

    std::unordered_map<std::string, std::vector<Vertex>> byTexture;
    Vector3i const chunkOffset = chunk.getPosition() * Vector3i{CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z};

    for (int x = 0; x < CHUNK_SIZE_X; ++x)
        for (int y = 0; y < CHUNK_SIZE_Y; ++y)
            for (int z = 0; z < CHUNK_SIZE_Z; ++z)
            {
                Block block = chunk.getBlock(x, y, z);
                if (!block.isSolid()) continue;

                Vector3i const blockLocalPos{x, y, z};
                Vector3i const blockWorldPos = blockLocalPos + chunkOffset;

                for (uint8_t face = 0; face < 6; ++face)
                {
                    Vector3i neighbor = blockLocalPos + FACE_NORMALS[face];
                    if (neighbor.x() >= 0 && neighbor.y() >= 0 && neighbor.z() >= 0 &&
                        neighbor.x() < CHUNK_SIZE_X && neighbor.y() < CHUNK_SIZE_Y && neighbor.z() < CHUNK_SIZE_Z &&
                        chunk.getBlock(neighbor.x(), neighbor.y(), neighbor.z()).isSolid())
                        continue;

                    std::string textureName = get_texture_name_for_block(block.type, face);

                    struct AO_Vertex
                    {
                        Vertex v;
                        float ao;
                    };
                    AO_Vertex verts[4];

                    float ao_sum = 0.0f;

                    for (int vi = 0; vi < 4; ++vi)
                    {
                        Vector3i offset = FACE_VERTICES[face][vi];
                        Vector3i vertexWorld = blockWorldPos + offset;
                        auto [side1Offset, side2Offset, cornerOffset] = AO_OFFSETS[face][vi];

                        Vector3i s1 = vertexWorld + side1Offset;
                        Vector3i s2 = vertexWorld + side2Offset;
                        Vector3i c = vertexWorld + cornerOffset;

                        bool b1 = get_block_solid(world, s1);
                        bool b2 = get_block_solid(world, s2);
                        bool bc = get_block_solid(world, c);

                        float ao = (b1 && b2) ? 0.0f : 1.0f - (b1 + b2 + bc) * 0.33f;

                        ao_sum += ao;

                        Vertex v;
                        v.position = Magnum::Vector3{vertexWorld};
                        v.normal = Magnum::Vector3{FACE_NORMALS[face]};
                        v.uv = FACE_UVS[face][vi];
                        v.ao = ao;

                        verts[vi] = AO_Vertex{v, ao};
                    }

                    float aoAvg = ao_sum / 4.0f;
                    for (int vi = 0; vi < 4; ++vi)
                        verts[vi].v.ao = Math::lerp(1.0f, aoAvg, 0.7f);

                    float diag1 = verts[0].ao + verts[2].ao;
                    float diag2 = verts[1].ao + verts[3].ao;

                    if (diag1 < diag2)
                    {
                        byTexture[textureName].push_back(verts[0].v);
                        byTexture[textureName].push_back(verts[1].v);
                        byTexture[textureName].push_back(verts[2].v);

                        byTexture[textureName].push_back(verts[0].v);
                        byTexture[textureName].push_back(verts[2].v);
                        byTexture[textureName].push_back(verts[3].v);
                    }
                    else
                    {
                        byTexture[textureName].push_back(verts[1].v);
                        byTexture[textureName].push_back(verts[2].v);
                        byTexture[textureName].push_back(verts[3].v);

                        byTexture[textureName].push_back(verts[1].v);
                        byTexture[textureName].push_back(verts[3].v);
                        byTexture[textureName].push_back(verts[0].v);
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
                std::move(buf), 0, attribute::POSITION_ATTRIBUTE, attribute::NORMAL_ATTRIBUTE, attribute::UV_ATTRIBUTE, attribute::AO_ATTRIBUTE);

        result.push_back({mesh, texName});
    }

    return result;
}
} // namespace mc::render
