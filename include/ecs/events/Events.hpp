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

struct ViewportResized
{
    float aspectRatio;
};

} // namespace mc::ecs
