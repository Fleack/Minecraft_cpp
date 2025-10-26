#pragma once

#include "utils/IVec3Hasher.hpp"

#include <functional>

#include <Magnum/Math/Vector3.h>

namespace mc::utils
{
struct PrioritizedChunk
{
    Magnum::Vector3i pos;
    float distance;

    bool operator<(PrioritizedChunk const& other) const
    {
        return distance > other.distance;
    }

    bool operator>(PrioritizedChunk const& other) const
    {
        return distance > other.distance;
    }

    bool operator==(PrioritizedChunk const& other) const
    {
        return pos == other.pos;
    }
};

struct PrioritizedChunkHasher
{
    std::size_t operator()(PrioritizedChunk const& chunk) const
    {
        return IVec3Hasher{}(chunk.pos);
    }
};
} // namespace mc::utils
