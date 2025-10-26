#pragma once

namespace mc::ecs
{

class ISystem
{
public:
    virtual ~ISystem() = default;

    virtual void update(float deltaTime) = 0;
};

} // namespace mc::ecs

