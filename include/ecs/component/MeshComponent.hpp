#pragma once

#include <memory>

namespace mc::render
{
class ChunkMesh;
}

namespace mc::ecs
{
struct MeshComponent
{
    std::shared_ptr<render::ChunkMesh> mesh;
};
}
