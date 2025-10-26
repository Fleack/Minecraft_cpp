#pragma once

#include "ecs/Entity.hpp"

#include <Magnum/Math/Vector3.h>

namespace mc::ecs
{

struct EntityCreated
{
    Entity entity;
};

struct EntityDestroyed
{
    Entity entity;
};

struct ChunkLoaded
{
    Magnum::Vector3i position;
};

struct ChunkUnloaded
{
    Magnum::Vector3i position;
};

} // namespace mc::ecs
