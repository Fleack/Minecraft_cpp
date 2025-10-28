#pragma once

#include "world/Chunk.hpp"
#include "world/IChunkProvider.hpp"

#include <optional>


namespace mc::world
{

class SimpleChunkProvider final : public IChunkProvider
{
public:
    SimpleChunkProvider();

    [[nodiscard]] std::optional<std::reference_wrapper<Chunk const>> getChunk(Magnum::Vector3i const& chunkPos) const override;

private:
    Chunk m_chunk;
};

} // namespace mc::world

