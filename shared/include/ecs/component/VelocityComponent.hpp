#pragma once

#include <Magnum/Math/Vector3.h>

namespace mc::ecs
{

struct VelocityComponent
{
    Magnum::Vector3d velocity{0.0f};
    Magnum::Vector3d desiredVelocity{0.0f};
    float maxSpeed{10.0f};
    float acceleration{7.0f};
    float damping{7.0f};
};

} // namespace mc::ecs
