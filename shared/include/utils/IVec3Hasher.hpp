#pragma once

#include <functional>

#include <Magnum/Math/Vector3.h>

namespace mc::utils
{
struct IVec3Hasher
{
    std::size_t operator()(Magnum::Vector3i const& v) const
    {
        // Hash each component individually
        std::size_t h1 = std::hash<int>()(v.x());
        std::size_t h2 = std::hash<int>()(v.y());
        std::size_t h3 = std::hash<int>()(v.z());

        // Combine the hashes using bitwise operations:
        //  - XOR to blend hash bits
        //  - shifts to spread entropy and reduce overlap
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};
} // namespace mc::utils
