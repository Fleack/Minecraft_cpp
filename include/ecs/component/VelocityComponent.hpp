#pragma once

#include <Magnum/Math/Vector3.h>

namespace mc::ecs
{

struct VelocityComponent
{
    Magnum::Vector3d velocity{0.0f};
    float speed{7.5f};
};

} // namespace mc::ecs
