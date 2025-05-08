#pragma once

#include "render/BlockTextureMapper.hpp"
#include "world/Chunk.hpp"

#include <unordered_map>
#include <vector>

namespace mc::world
{
class World;
}

namespace mc::ecs
{
struct MeshComponent;
}

namespace mc::render
{
struct Vertex;

static constexpr int FACE_COUNT = 6;
static constexpr int VERTS_PER_FACE = 4;

/**
 * @brief Generates a mesh for a single chunk using visible block faces.
 */
class ChunkMeshBuilder
{
public:
    using OffsetTuple = std::tuple<Magnum::Vector3i, Magnum::Vector3i, Magnum::Vector3i>;

    /**
     * @brief Builds a mesh from the given chunk.
     *
     * @param chunk Reference to the voxel chunk.
     */
    static std::vector<ecs::MeshComponent> build(world::Chunk const& chunk, world::World const& world);

private:
    static void collectVertices(world::Chunk const& chunk, world::World const& world, std::unordered_map<texture_id, std::vector<Vertex>>& out);

    static void processBlock(
        world::World const& world,
        world::Block const& block,
        Magnum::Vector3i const& worldPos,
        std::unordered_map<texture_id, std::vector<Vertex>>& out);

    static std::array<Vertex, VERTS_PER_FACE> computeFaceVertices(
        world::World const& world,
        Magnum::Vector3i const& worldPos,
        int face);

    static void adjustAo(std::array<Vertex, VERTS_PER_FACE>& faceVerts);

    static void appendTriangles(std::vector<Vertex>& out, std::array<Vertex, VERTS_PER_FACE> const& faceVerts);

    static std::vector<ecs::MeshComponent> buildMeshComponents(std::unordered_map<texture_id, std::vector<Vertex>> const& vertsByTex);

    static bool isWorldBlockSolid(world::World const& world, Magnum::Vector3i const& pos);
    static float computeAo(world::World const& world, Magnum::Vector3i const& vertexPos, OffsetTuple const& offsets);
};

} // namespace mc::render
