#pragma once

namespace mc::ecs
{
class ISystem
{
public:
    virtual void Update(float deltaTime) = 0;
    virtual ~ISystem() = default;
};
}
