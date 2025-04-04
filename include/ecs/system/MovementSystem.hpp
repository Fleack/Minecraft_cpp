#pragma once

#include "ISystem.hpp"

namespace mc::ecs
{
class ECS;

class MovementSystem final : public ISystem
{
public:
    explicit MovementSystem(ECS& ecs);

    void Update(float deltaTime) override;

private:
    ECS& m_ecs;
};
}
