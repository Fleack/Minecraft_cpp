#include "render/ChunkMeshBuilder.hpp"

#include "ecs/component/MeshComponent.hpp"
#include "render/BlockTextureMapper.hpp"
#include "render/Vertex.hpp"
#include "utils/FastDivFloor.hpp"
#include "world/World.hpp"

#include <array>
#include <memory>
#include <string>
#include <unordered_map>

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Functions.h>
#include <Magnum/Math/Vector3.h>

namespace
{
constexpr std::array<Magnum::Vector3i, mc::render::FACE_COUNT> FACE_NORMALS{{
    {0, 0, 1}, // front
    {0, 0, -1}, // back
    {0, 1, 0}, // top
    {0, -1, 0}, // bottom
    {1, 0, 0}, // right
    {-1, 0, 0}, // left
}};

constexpr std::array<std::array<Magnum::Vector3i, mc::render::VERTS_PER_FACE>, mc::render::FACE_COUNT> FACE_OFFSETS{{
    {{{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}}}, // front
    {{{1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {1, 1, 0}}}, // back
    {{{0, 1, 1}, {1, 1, 1}, {1, 1, 0}, {0, 1, 0}}}, // top
    {{{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}}}, // bottom
    {{{1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 1}}}, // right
    {{{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0}}}, // left
}};

constexpr std::array<std::array<Magnum::Vector2, mc::render::VERTS_PER_FACE>, mc::render::FACE_COUNT> FACE_UVS{{
    {{{0, 0}, {1, 0}, {1, 1}, {0, 1}}}, // front
    {{{1, 0}, {0, 0}, {0, 1}, {1, 1}}}, // back
    {{{0, 1}, {1, 1}, {1, 0}, {0, 0}}}, // top
    {{{0, 0}, {1, 0}, {1, 1}, {0, 1}}}, // bottom
    {{{0, 0}, {1, 0}, {1, 1}, {0, 1}}}, // right
    {{{0, 0}, {1, 0}, {1, 1}, {0, 1}}}, // left
}};

constexpr std::array<std::array<mc::render::ChunkMeshBuilder::OffsetTuple, mc::render::VERTS_PER_FACE>, mc::render::FACE_COUNT> AO_OFFSETS{{
    // front
    {{{{0, -1, 0}, {-1, 0, 0}, {-1, -1, 1}},
      {{0, -1, 0}, {1, 0, 0}, {1, -1, 1}},
      {{0, 1, 0}, {1, 0, 0}, {1, 1, 1}},
      {{0, 1, 0}, {-1, 0, 0}, {-1, 1, 1}}}},
    // back
    {{{{0, -1, 0}, {1, 0, 0}, {1, -1, -1}},
      {{0, -1, 0}, {-1, 0, 0}, {-1, -1, -1}},
      {{0, 1, 0}, {-1, 0, 0}, {-1, 1, -1}},
      {{0, 1, 0}, {1, 0, 0}, {1, 1, -1}}}},
    // top
    {{{{0, 0, 1}, {-1, 0, 0}, {-1, 0, 1}},
      {{0, 0, 1}, {1, 0, 0}, {1, 0, 1}},
      {{0, 0, -1}, {1, 0, 0}, {1, 0, -1}},
      {{0, 0, -1}, {-1, 0, 0}, {-1, 0, -1}}}},
    // bottom
    {{{{0, 0, -1}, {-1, 0, 0}, {-1, 0, -1}},
      {{0, 0, -1}, {1, 0, 0}, {1, 0, -1}},
      {{0, 0, 1}, {1, 0, 0}, {1, 0, 1}},
      {{0, 0, 1}, {-1, 0, 0}, {-1, 0, 1}}}},
    // right
    {{{{0, -1, 0}, {0, 0, 1}, {1, -1, 1}},
      {{0, -1, 0}, {0, 0, -1}, {1, -1, -1}},
      {{0, 1, 0}, {0, 0, -1}, {1, 1, -1}},
      {{0, 1, 0}, {0, 0, 1}, {1, 1, 1}}}},
    // left
    {{{{0, -1, 0}, {0, 0, -1}, {-1, -1, -1}},
      {{0, -1, 0}, {0, 0, 1}, {-1, -1, 1}},
      {{0, 1, 0}, {0, 0, 1}, {-1, 1, 1}},
      {{0, 1, 0}, {0, 0, -1}, {-1, 1, -1}}}},
}};
} // namespace

namespace mc::render
{

std::vector<std::vector<Vertex>> ChunkMeshBuilder::buildVertexData(
    world::Chunk const& chunk,
    world::World const& world)
{
    CachedChunksMap chunks;
    Magnum::Vector3i center = chunk.getPosition();
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            for (int dz = -1; dz <= 1; ++dz)
            {
                Magnum::Vector3i pos = center + Magnum::Vector3i{dx, dy, dz};
                if (auto opt = world.getChunk(pos))
                {
                    chunks[pos] = &opt->get();
                }
            }
        }
    }

    std::vector<std::vector<Vertex>> vertsByTexture(g_max_texture_id);
    collectVertices(chunk, chunks, vertsByTexture);
    return vertsByTexture;
}

void ChunkMeshBuilder::collectVertices(world::Chunk const& chunk, CachedChunksMap const& chunks, std::vector<std::vector<Vertex>>& out)
{
    using namespace world;
    static constexpr Magnum::Vector3i chunkSize{CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z};
    Magnum::Vector3i chunkOffset = chunk.getPosition() * chunkSize;
    for (int x = 0; x < CHUNK_SIZE_X; ++x)
    {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y)
        {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z)
            {
                auto block = chunk.getBlock(x, y, z);
                if (!block.isSolid()) continue;

                Magnum::Vector3i worldBlockPos = {Magnum::Vector3i{x, y, z} + chunkOffset};
                processBlock(chunks, block, worldBlockPos, out);
            }
        }
    }
}

void ChunkMeshBuilder::processBlock(
    CachedChunksMap const& chunks,
    world::Block const& block,
    Magnum::Vector3i const& worldPos,
    std::vector<std::vector<Vertex>>& out)
{
    for (int face = 0; face < FACE_COUNT; ++face)
    {
        if (isWorldBlockSolid(chunks, worldPos + FACE_NORMALS[face]))
            continue;

        auto textureName = get_texture_name_for_block(block.type, face);
        auto textureId = get_texture_id_by_name(textureName);

        if (textureId >= out.size())
        {
            std::abort();
        }

        auto faceVerts = computeFaceVertices(chunks, worldPos, face);
        adjustAo(faceVerts);
        appendTriangles(out[textureId], faceVerts);
    }
}

std::array<Vertex, VERTS_PER_FACE> ChunkMeshBuilder::computeFaceVertices(CachedChunksMap const& chunks, Magnum::Vector3i const& worldPos, int face)
{
    std::array<Vertex, VERTS_PER_FACE> verts;
    for (int i = 0; i < VERTS_PER_FACE; ++i)
    {
        Magnum::Vector3i vPos = worldPos + FACE_OFFSETS[face][i];
        float ao = computeAo(chunks, vPos, AO_OFFSETS[face][i]);
        verts[i] = {
            Magnum::Vector3{vPos},
            Magnum::Vector3{FACE_NORMALS[face]},
            FACE_UVS[face][i],
            ao};
    }
    return verts;
}

void ChunkMeshBuilder::adjustAo(std::array<Vertex, VERTS_PER_FACE>& faceVerts)
{
    float sumAo = std::accumulate(
        faceVerts.begin(), faceVerts.end(), 0.0f, [](float acc, Vertex const& v) { return acc + v.ao; });
    float avgAo = sumAo / static_cast<float>(VERTS_PER_FACE);

    for (auto& v : faceVerts)
    {
        v.ao = Magnum::Math::lerp(1.0f, avgAo, 0.7f);
    }
}

void ChunkMeshBuilder::appendTriangles(std::vector<Vertex>& out, std::array<Vertex, VERTS_PER_FACE> const& faceVerts)
{
    float d1 = faceVerts[0].ao + faceVerts[2].ao;
    float d2 = faceVerts[1].ao + faceVerts[3].ao;
    if (d1 < d2)
    {
        out.insert(out.end(), {faceVerts[0], faceVerts[1], faceVerts[2], faceVerts[0], faceVerts[2], faceVerts[3]});
    }
    else
    {
        out.insert(out.end(), {faceVerts[1], faceVerts[2], faceVerts[3], faceVerts[1], faceVerts[3], faceVerts[0]});
    }
}

std::vector<ecs::MeshComponent> ChunkMeshBuilder::buildMeshComponents(std::vector<std::vector<Vertex>> const& vertsByTex)
{
    std::vector<ecs::MeshComponent> result;
    result.reserve(vertsByTex.size());

    for (texture_id tex = 0; tex < vertsByTex.size(); ++tex)
    {
        auto const& verts = vertsByTex[tex];
        if (verts.empty()) continue;

        auto mesh = std::make_shared<Magnum::GL::Mesh>();
        Magnum::GL::Buffer buf;
        buf.setData(
            Magnum::Containers::arrayView(verts.data(), verts.size()),
            Magnum::GL::BufferUsage::StaticDraw);

        mesh->setPrimitive(Magnum::GL::MeshPrimitive::Triangles)
            .setCount(verts.size())
            .addVertexBuffer(
                std::move(buf),
                0,
                attribute::POSITION_ATTRIBUTE,
                attribute::NORMAL_ATTRIBUTE,
                attribute::UV_ATTRIBUTE,
                attribute::AO_ATTRIBUTE);

        result.emplace_back(mesh, tex);
    }
    return result;
}

std::optional<world::Block> ChunkMeshBuilder::getBlockAt(Magnum::Vector3i worldPos, CachedChunksMap const& chunks)
{
    using namespace world;

    Magnum::Vector3i chunkPos{
        utils::floor_div(worldPos.x(), CHUNK_SIZE_X),
        utils::floor_div(worldPos.y(), CHUNK_SIZE_Y),
        utils::floor_div(worldPos.z(), CHUNK_SIZE_Z)};

    Magnum::Vector3i local{
        worldPos.x() - chunkPos.x() * CHUNK_SIZE_X,
        worldPos.y() - chunkPos.y() * CHUNK_SIZE_Y,
        worldPos.z() - chunkPos.z() * CHUNK_SIZE_Z};

    auto it = chunks.find(chunkPos);
    if (it == chunks.end()) return std::nullopt;

    return it->second->getBlock(local.x(), local.y(), local.z());
}

bool ChunkMeshBuilder::isWorldBlockSolid(CachedChunksMap const& chunks, Magnum::Vector3i const& pos)
{
    if (auto block = getBlockAt(pos, chunks))
    {
        return block->isSolid();
    }
    return false;
}

float ChunkMeshBuilder::computeAo(CachedChunksMap const& chunks, Magnum::Vector3i const& vertexPos, OffsetTuple const& offsets)
{
    auto [off1, off2, offC] = offsets;
    bool s1 = isWorldBlockSolid(chunks, vertexPos + off1);
    bool s2 = isWorldBlockSolid(chunks, vertexPos + off2);
    bool sc = isWorldBlockSolid(chunks, vertexPos + offC);
    return (s1 && s2)
        ? 0.0f
        : 1.0f - (static_cast<float>(s1 + s2 + sc) * 0.33f);
}

} // namespace mc::render
