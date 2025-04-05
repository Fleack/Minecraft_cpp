#pragma once

#include <functional>

#include <glm/vec3.hpp>

namespace mc::utils
{
struct IVec3Hasher
{
    std::size_t operator()(glm::ivec3 const& v) const
    {
        std::size_t h1 = std::hash<int>()(v.x);
        std::size_t h2 = std::hash<int>()(v.y);
        std::size_t h3 = std::hash<int>()(v.z);
        return ((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1);
    }
};
}
