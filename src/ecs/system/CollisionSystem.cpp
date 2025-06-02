#include "ecs/system/CollisionSystem.hpp"

#include "core/Logger.hpp"
#include "ecs/component/ColliderComponent.hpp"
#include "ecs/component/PlayerComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/component/VelocityComponent.hpp"
#include "world/Chunk.hpp"
#include "world/World.hpp"

#include <cmath>

#include <Magnum/Math/Functions.h>

namespace mc::ecs
{

// ReSharper disable once CppInconsistentNaming
struct AABB
{
    Magnum::Vector3d min;
    Magnum::Vector3d max;

    static constexpr double epsilon = 1e-6;

    static constexpr AABB fromCenterHalfExtents(Magnum::Vector3d const& center, Magnum::Vector3d const& halfExtents) noexcept
    {
        return {center - halfExtents, center + halfExtents};
    }

    constexpr bool intersects(const AABB& other) const noexcept
    {
        return (min.x() < other.max.x() - epsilon && max.x() > other.min.x() + epsilon) &&
            (min.y() < other.max.y() - epsilon && max.y() > other.min.y() + epsilon) &&
            (min.z() < other.max.z() - epsilon && max.z() > other.min.z() + epsilon);
    }
};

CollisionSystem::CollisionSystem(Ecs& ecs, world::World& world)
    : ISystem{Type::COLLISION}, m_ecs(ecs), m_world(world)
{
    LOG(INFO, "CollisionSystem initialized");
}

bool CollisionSystem::isSolidAt(Magnum::Vector3d const& pos) const
{
    using namespace world;
    auto blockPos = static_cast<Magnum::Vector3i>(Magnum::Math::floor(pos));
    auto chunkPos = World::getChunkOfPosition(blockPos);

    auto chunkOpt = m_world.getChunk(chunkPos);
    if (!chunkOpt) return true;

    int const localX = blockPos.x() & (CHUNK_SIZE_X - 1);
    int const localY = blockPos.y() & (CHUNK_SIZE_Y - 1);
    int const localZ = blockPos.z() & (CHUNK_SIZE_Z - 1);

    return chunkOpt->get().getBlock(localX, localY, localZ).isSolid();
}

bool CollisionSystem::collides(const AABB& box) const
{
    auto min = static_cast<Magnum::Vector3i>(Magnum::Math::floor(box.min));
    auto max = static_cast<Magnum::Vector3i>(Magnum::Math::floor(box.max));

    for (int x = min.x(); x <= max.x(); ++x)
        for (int y = min.y(); y <= max.y(); ++y)
            for (int z = min.z(); z <= max.z(); ++z)
            {
                Magnum::Vector3d blockCenter{x + 0.5, y + 0.5, z + 0.5};
                if (!isSolidAt(blockCenter)) continue;
                AABB block{
                    {static_cast<double>(x), static_cast<double>(y), static_cast<double>(z)},
                    {static_cast<double>(x + 1), static_cast<double>(y + 1), static_cast<double>(z + 1)}};
                if (box.intersects(block))
                    return true;
            }

    return false;
}

// ReSharper disable once CppInconsistentNaming
Magnum::Vector3d CollisionSystem::sweepAABB(
    Magnum::Vector3d const& pos,
    Magnum::Vector3d const& vel,
    Magnum::Vector3d const& halfExtents,
    float dt)
{
    Magnum::Vector3d velocity = vel;
    AABB box = AABB::fromCenterHalfExtents(pos, halfExtents);
    static constexpr double epsilon = 1e-7;

    for (int axis = 0; axis < 3; ++axis)
    {
        double move = velocity[axis] * dt;
        if (Magnum::Math::abs(move) < epsilon)
            continue;

        double sign = move > 0.0 ? 1.0 : -1.0;
        double maxMove = move;

        AABB expanded = box;
        expanded.min[axis] += std::min(0.0, move);
        expanded.max[axis] += std::max(0.0, move);

        auto min = static_cast<Magnum::Vector3i>(Magnum::Math::floor(expanded.min));
        auto max = static_cast<Magnum::Vector3i>(Magnum::Math::floor(expanded.max));

        int axis1 = (axis + 1) % 3;
        int axis2 = (axis + 2) % 3;

        for (int x = min.x(); x <= max.x(); ++x)
            for (int y = min.y(); y <= max.y(); ++y)
                for (int z = min.z(); z <= max.z(); ++z)
                {
                    Magnum::Vector3d blockMin{Magnum::Vector3i{x, y, z}};
                    Magnum::Vector3d blockMax = blockMin + Magnum::Vector3d{1.0};

                    if (!isSolidAt(blockMin + Magnum::Vector3d{0.5})) continue;

                    if (box.max[axis1] <= blockMin[axis1] || box.min[axis1] >= blockMax[axis1]) continue;
                    if (box.max[axis2] <= blockMin[axis2] || box.min[axis2] >= blockMax[axis2]) continue;

                    double blockFace = sign > 0.0 ? blockMin[axis] : blockMax[axis];
                    double boxFace = sign > 0.0 ? box.max[axis] : box.min[axis];
                    double distance = blockFace - boxFace;

                    if (sign * distance < sign * maxMove)
                    {
                        maxMove = distance - sign * epsilon;
                    }
                }

        if (maxMove != move)
        {
            velocity[axis] = maxMove / dt;
            if (std::abs(velocity[axis]) < epsilon)
                velocity[axis] = 0.0;
        }

        box.min[axis] += maxMove;
        box.max[axis] += maxMove;
    }

    return velocity;
}

bool CollisionSystem::isOnGround(Magnum::Vector3d const& pos, Magnum::Vector3d const& halfExtents) const
{
    constexpr double epsilon = 0.05;

    double checkY = std::floor(pos.y() - halfExtents.y() - epsilon);
    int minX = std::floor(pos.x() - halfExtents.x());
    int maxX = std::floor(pos.x() + halfExtents.x());
    int minZ = std::floor(pos.z() - halfExtents.z());
    int maxZ = std::floor(pos.z() + halfExtents.z());

    for (int x = minX; x <= maxX; ++x)
    {
        for (int z = minZ; z <= maxZ; ++z)
        {
            Magnum::Vector3d blockCenter{x + 0.5, checkY + 0.5, z + 0.5};
            if (isSolidAt(blockCenter))
                return true;
        }
    }
    return false;
}

void CollisionSystem::update(float dt)
{
    for (auto& [entity, collider] : m_ecs.getAllComponents<ColliderComponent>())
    {
        auto transform = m_ecs.getComponent<TransformComponent>(entity);
        auto velocity = m_ecs.getComponent<VelocityComponent>(entity);
        auto player = m_ecs.getComponent<PlayerComponent>(entity);
        if (!transform || !velocity || !player) continue;

        auto pos = transform->position;
        auto vel = velocity->velocity;
        auto halfExtents = collider.halfExtents;

        velocity->velocity = sweepAABB(pos, vel, halfExtents, dt);
        player->onGround = isOnGround(pos, halfExtents);
    }
}

} // namespace mc::ecs
