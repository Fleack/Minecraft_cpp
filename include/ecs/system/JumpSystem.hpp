#pragma once

#include "ecs/Ecs.hpp"

namespace mc::ecs
{

class JumpSystem final : public ISystem
{
public:
    explicit JumpSystem(Ecs& ecs);

    void update(float dt) override;
    void render(float) override {}

private:
    Ecs& m_ecs;

    static constexpr float jumpForce = 7.5f;
};

} // namespace mc::ecs
