#pragma once

#include <Magnum/Math/Vector3.h>

namespace mc::ecs
{

struct VelocityComponent
{
    Magnum::Vector3 velocity{0.0f};
};

} // namespace mc::ecs
