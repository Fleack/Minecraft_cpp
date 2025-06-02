#pragma once

#include "ecs/Entity.hpp"

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

} // namespace mc::ecs
