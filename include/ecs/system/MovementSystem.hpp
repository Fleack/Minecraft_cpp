#pragma once

#include "ISystem.hpp"

namespace mc::ecs
{
class Ecs;

class MovementSystem final : public ISystem
{
public:
    explicit MovementSystem(Ecs& ecs);

    void update(float deltaTime) override;

private:
    Ecs& m_ecs;
};
}
