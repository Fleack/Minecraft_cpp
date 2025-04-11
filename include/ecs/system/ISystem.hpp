#pragma once

namespace mc::ecs
{
class ISystem
{
public:
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual ~ISystem() = default;
};
}
