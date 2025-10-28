#pragma once

#include <ecs/Ecs.hpp>
#include <ecs/system/ISystem.hpp>

namespace mc::ecs
{

class GravitySystem final : public ISystem
{
public:
    explicit GravitySystem(Ecs& ecs);

    void update(float dt) override;

private:
    Ecs& m_ecs;
    static constexpr float GRAVITY = 9.81f;
    static constexpr float TERMINAL_VELOCITY = -25.0f;
};

} // namespace mc::ecs
