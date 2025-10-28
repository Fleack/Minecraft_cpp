#pragma once

#include <functional>
#include <optional>

#include <Magnum/Math/Vector3.h>
#include <world/Chunk.hpp>

namespace mc::world
{

class IChunkProvider
{
public:
    virtual ~IChunkProvider() = default;
    [[nodiscard]] virtual std::optional<std::reference_wrapper<Chunk const>> getChunk(Magnum::Vector3i const& chunkPos) const = 0;
};

} // namespace mc::world
