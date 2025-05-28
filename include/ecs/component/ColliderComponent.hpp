#pragma once

#include <Magnum/Math/Vector3.h>

namespace mc::ecs
{

struct ColliderComponent
{
    Magnum::Vector3d halfExtents = {0.3, 0.9, 0.3}; // Player half (0.6 x 1.8 x 0.6)
};

} // namespace mc::ecs
