#pragma once

#include <ecs/Ecs.hpp>
#include <ecs/system/ISystem.hpp>

namespace mc::ecs
{

class JumpSystem final : public ISystem
{
public:
    explicit JumpSystem(Ecs& ecs);

    void update(float dt) override;

private:
    Ecs& m_ecs;

    static constexpr float JUMP_FORCE = 7.5f;
};

} // namespace mc::ecs
