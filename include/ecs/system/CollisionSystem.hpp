#pragma once

#include "ecs/Ecs.hpp"
#include "world/World.hpp"

namespace mc::ecs
{

class CollisionSystem final : public ISystem
{
public:
    struct SweepResult
    {
        Magnum::Vector3d position;
        Magnum::Vector3d velocity;
    };

    CollisionSystem(Ecs& ecs, world::World& world);

    void update(float dt) override;
    void render(float) override {}

private:
    bool isSolidAt(Magnum::Vector3d const& pos) const;
    SweepResult sweepAABB(
        Magnum::Vector3d const& pos,
        Magnum::Vector3d const& vel,
        Magnum::Vector3d const& halfExtents,
        float dt);
    bool isOnGround(Magnum::Vector3d const& pos, Magnum::Vector3d const& halfExtents) const;

private:
    Ecs& m_ecs;
    world::World& m_world;
};

} // namespace mc::ecs
