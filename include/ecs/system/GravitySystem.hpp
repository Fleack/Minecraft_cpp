#pragma once

#include "ecs/Ecs.hpp"

namespace mc::ecs
{

class GravitySystem final : public ISystem
{
public:
    explicit GravitySystem(Ecs& ecs);

    void update(float dt) override;
    void render(float) override {}

private:
    Ecs& m_ecs;
    constexpr static float gravity = 9.81f;
};

} // namespace mc::ecs
