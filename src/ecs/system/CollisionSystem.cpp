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

struct AABB
{
    Magnum::Vector3d min;
    Magnum::Vector3d max;

    static AABB fromCenterHalfExtents(Magnum::Vector3d const& center, Magnum::Vector3d const& halfExtents)
    {
        return {center - halfExtents, center + halfExtents};
    }

    bool intersects(const AABB& other) const
    {
        constexpr double eps = 1e-8;
        return (min.x() < other.max.x() - eps && max.x() > other.min.x() + eps) &&
            (min.y() < other.max.y() - eps && max.y() > other.min.y() + eps) &&
            (min.z() < other.max.z() - eps && max.z() > other.min.z() + eps);
    }

    void move(Magnum::Vector3d const& offset)
    {
        min += offset;
        max += offset;
    }

    AABB moved(Magnum::Vector3d const& offset) const
    {
        return {min + offset, max + offset};
    }
};

CollisionSystem::CollisionSystem(Ecs& ecs, world::World& world)
    : ISystem{Type::COLLISION}, m_ecs(ecs), m_world(world) {}

bool CollisionSystem::isSolidAt(Magnum::Vector3d const& pos) const
{
    using namespace world;

    auto blockPos = Magnum::Vector3i{Magnum::Math::floor(pos)};
    Magnum::Vector3i chunkPos = World::getChunkOfPosition(blockPos);
    Magnum::Vector3i localPos = {
        (blockPos.x() % CHUNK_SIZE_X + CHUNK_SIZE_X) % CHUNK_SIZE_X,
        (blockPos.y() % CHUNK_SIZE_Y + CHUNK_SIZE_Y) % CHUNK_SIZE_Y,
        (blockPos.z() % CHUNK_SIZE_Z + CHUNK_SIZE_Z) % CHUNK_SIZE_Z};

    auto chunkOpt = m_world.getChunk(chunkPos);
    if (!chunkOpt) return true;
    auto& chunk = chunkOpt->get();
    auto block = chunk.getBlock(localPos.x(), localPos.y(), localPos.z());
    return block.isSolid();
}

CollisionSystem::SweepResult CollisionSystem::sweepAABB(
    Magnum::Vector3d const& pos,
    Magnum::Vector3d const& vel,
    Magnum::Vector3d const& halfExtents,
    float dt)
{
    constexpr double skin = 0.1;
    constexpr double epsilon = 0.000001;

    Magnum::Vector3d velocity = vel;
    Magnum::Vector3d position = pos;
    AABB box = AABB::fromCenterHalfExtents(pos, halfExtents);

    for (int axis = 0; axis < 3; ++axis)
    {
        double move = velocity[axis] * dt;
        if (std::abs(move) < epsilon)
        {
            continue;
        }

        double sign = (move > 0.0) ? 1.0 : -1.0;
        double maxDist = std::abs(move);

        AABB expanded = box;
        expanded.min[axis] += std::min(0.0, move);
        expanded.max[axis] += std::max(0.0, move);

        int axis1 = (axis + 1) % 3;
        int axis2 = (axis + 2) % 3;

        int minX = int(std::floor(expanded.min.x()));
        int maxX = int(std::floor(expanded.max.x()));
        int minY = int(std::floor(expanded.min.y()));
        int maxY = int(std::floor(expanded.max.y()));
        int minZ = int(std::floor(expanded.min.z()));
        int maxZ = int(std::floor(expanded.max.z()));
        for (int x = minX; x <= maxX; ++x)
        {
            for (int y = minY; y <= maxY; ++y)
            {
                for (int z = minZ; z <= maxZ; ++z)
                {
                    Magnum::Vector3d blockPos{(double)x, (double)y, (double)z};
                    if (!isSolidAt(blockPos)) continue;

                    AABB block = {blockPos, blockPos + Magnum::Vector3d{1.0}};

                    if (expanded.max[axis1] > block.min[axis1] &&
                        expanded.min[axis1] < block.max[axis1] &&
                        expanded.max[axis2] > block.min[axis2] &&
                        expanded.min[axis2] < block.max[axis2])
                    {
                        double boxFace = (sign > 0.0) ? box.max[axis] : box.min[axis];
                        double blockFace = (sign > 0.0) ? block.min[axis] : block.max[axis];
                        double dist = blockFace - boxFace - skin;

                        LOG(DEBUG, "[AXIS {}] Overlap block at [{}, {}, {}], dist = {}", axis, x, y, z, dist);

                        if (dist * sign > 0.0 && std::abs(dist) < std::abs(maxDist))
                        {
                            maxDist = dist;
                            LOG(DEBUG, "[AXIS {}] Updating maxDist = {}", axis, maxDist);
                        }
                    }
                }
            }
        }

        double actualMove = sign * maxDist;
        position[axis] += actualMove;

        if (maxDist < std::abs(move))
        {
            LOG(DEBUG, "[AXIS {}] Collision occurred, zero velocity", axis);
            velocity[axis] = 0.0;
        }

        box = AABB::fromCenterHalfExtents(position, halfExtents);
    }

    return {position, velocity};
}

bool CollisionSystem::isOnGround(Magnum::Vector3d const& pos, Magnum::Vector3d const& halfExtents) const
{
    constexpr double eps = 0.05;
    Magnum::Vector3d bbMin = pos - halfExtents;
    Magnum::Vector3d bbMax = pos + halfExtents;
    bbMin.y() -= eps;
    bbMax.y() = bbMin.y();

    int minX = std::floor(bbMin.x());
    int maxX = std::floor(bbMax.x());
    int minZ = std::floor(bbMin.z());
    int maxZ = std::floor(bbMax.z());
    int blockY = std::floor(bbMin.y());

    for (int x = minX; x <= maxX; ++x)
    {
        for (int z = minZ; z <= maxZ; ++z)
        {
            if (isSolidAt({x + 0.5, blockY + 0.5, z + 0.5}))
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

        int steps = std::max(1, int(velocity->velocity.length() * dt * 10));
        float subdt = dt / steps;
        LOG(DEBUG, "pos = {},{},{} vel = {},{},{}", pos.x(), pos.y(), pos.z(), vel.x(), vel.y(), vel.z());
        for (int i = 0; i < steps; ++i)
        {
            auto [newPos, newVel] = sweepAABB(pos, vel, halfExtents, subdt);
            pos = newPos;
            vel = newVel;
        }
        LOG(DEBUG, "pos = {},{},{} vel = {},{},{}", pos.x(), pos.y(), pos.z(), vel.x(), vel.y(), vel.z());

        transform->position = pos;
        velocity->velocity = vel;
        player->onGround = isOnGround(pos, halfExtents);
    }
}

} // namespace mc::ecs
