#pragma once

#include "ISystem.hpp"

namespace mc::ecs
{
class ECS;

class MovementSystem final : public ISystem
{
public:
    explicit MovementSystem(ECS& ecs);

    void update(float deltaTime) override;

private:
    ECS& m_ecs;
};
}
