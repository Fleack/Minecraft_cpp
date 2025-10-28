#pragma once

#include <ecs/system/ISystem.hpp>

namespace mc::ecs
{

class IRenderableSystem : public ISystem
{
public:
    ~IRenderableSystem() override = default;

    virtual void render(float deltaTime) = 0;
};

} // namespace mc::ecs

