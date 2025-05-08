#pragma once

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
        std::size_t h1 = std::hash<int>()(chunk.pos.x());
        std::size_t h2 = std::hash<int>()(chunk.pos.y());
        std::size_t h3 = std::hash<int>()(chunk.pos.z());
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};
} // namespace mc::utils
