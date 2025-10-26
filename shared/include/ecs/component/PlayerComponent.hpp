#pragma once

namespace mc::ecs
{

struct PlayerComponent
{
    bool onGround = false;
    bool wantsToJump = false;
};

} // namespace mc::ecs
