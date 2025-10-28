#pragma once

#include "ecs/Ecs.hpp"
#include "world/World.hpp"

namespace mc::ecs
{
struct AABB;

class CollisionSystem final : public ISystem
{
public:
    CollisionSystem(Ecs& ecs, world::World& world);

    void update(float dt) override;

private:
    bool collides(const AABB& box) const;
    bool isSolidAt(Magnum::Vector3d const& pos) const;
    Magnum::Vector3d sweepAABB(
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
