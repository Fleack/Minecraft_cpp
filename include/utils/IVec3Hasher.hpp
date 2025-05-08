#pragma once

#include <functional>

#include <Magnum/Math/Vector3.h>

namespace mc::utils
{
/**
 * @brief Hash function for glm::ivec3.
 *
 * Allows glm::ivec3 to be used as a key in unordered containers like std::unordered_map.
 */
struct IVec3Hasher
{
    /**
     * @brief Computes a hash for a 3D integer vector.
     *
     * @param v The vector to hash.
     *
     * @return Hash value.
     */
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
