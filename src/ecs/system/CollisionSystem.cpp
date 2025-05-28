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
        constexpr double eps = 1e-6;
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
    return chunk.getBlock(localPos.x(), localPos.y(), localPos.z()).isSolid();
}

bool CollisionSystem::collides(const AABB& box) const
{
    Magnum::Vector3d minD = Magnum::Math::floor(box.min);
    Magnum::Vector3d maxD = Magnum::Math::floor(box.max);

    Magnum::Vector3i min{int(minD.x()), int(minD.y()), int(minD.z())};
    Magnum::Vector3i max{int(maxD.x()), int(maxD.y()), int(maxD.z())};

    for (int x = min.x(); x <= max.x(); ++x)
        for (int y = min.y(); y <= max.y(); ++y)
            for (int z = min.z(); z <= max.z(); ++z)
            {
                if (!isSolidAt({x + 0.5, y + 0.5, z + 0.5})) continue;

                AABB block{{(double)x, (double)y, (double)z}, {x + 1.0, y + 1.0, z + 1.0}};
                if (box.intersects(block))
                    return true;
            }

    return false;
}

CollisionSystem::SweepResult CollisionSystem::sweepAABB(
    Magnum::Vector3d const& pos,
    Magnum::Vector3d const& vel,
    Magnum::Vector3d const& halfExtents,
    float dt)
{
    Magnum::Vector3d position = pos;
    Magnum::Vector3d velocity = vel;

    for (int axis = 0; axis < 3; ++axis)
    {
        AABB box = AABB::fromCenterHalfExtents(position, halfExtents);
        double move = velocity[axis] * dt;
        if (move == 0.0)
            continue;

        double sign = move > 0.0 ? 1.0 : -1.0;
        double maxMove = move;

        AABB expanded = box;
        expanded.min[axis] += std::min(0.0, move);
        expanded.max[axis] += std::max(0.0, move);

        int axis1 = (axis + 1) % 3;
        int axis2 = (axis + 2) % 3;

        Magnum::Vector3d minD = Magnum::Math::floor(expanded.min);
        Magnum::Vector3d maxD = Magnum::Math::floor(expanded.max);

        Magnum::Vector3i min{int(minD.x()), int(minD.y()), int(minD.z())};
        Magnum::Vector3i max{int(maxD.x()), int(maxD.y()), int(maxD.z())};
        LOG(DEBUG, "[AXIS {}] Start sweep: move = {}, sign = {}", axis, move, sign);

        for (int x = min.x(); x <= max.x(); ++x)
            for (int y = min.y(); y <= max.y(); ++y)
                for (int z = min.z(); z <= max.z(); ++z)
                {
                    Magnum::Vector3d blockMin{double(x), double(y), double(z)};
                    Magnum::Vector3d blockMax = blockMin + Magnum::Vector3d{1.0};
                    LOG(DEBUG, "[AXIS {}] Checking block {}, {}, {}", axis, x, y, z);

                    if (!isSolidAt(blockMin + Magnum::Vector3d{0.5})) continue;

                    if (box.max[axis1] <= blockMin[axis1] || box.min[axis1] >= blockMax[axis1]) continue;
                    if (box.max[axis2] <= blockMin[axis2] || box.min[axis2] >= blockMax[axis2]) continue;
                    LOG(DEBUG, "[AXIS {}] Potential collision with block [{}, {}, {}]", axis, x, y, z);

                    double blockFace = sign > 0.0 ? blockMin[axis] : blockMax[axis];
                    double boxFace = sign > 0.0 ? box.max[axis] : box.min[axis];
                    double distance = blockFace - boxFace;
                    LOG(DEBUG, "[AXIS {}] boxFace = {}, blockFace = {}, distance = {}, old maxMove = {}", axis, boxFace, blockFace, distance, maxMove);

                    if (sign > 0.0)
                    {
                        if (distance < maxMove) maxMove = distance;
                    }
                    else
                    {
                        if (distance > maxMove) maxMove = distance;
                    }
                    LOG(DEBUG, "[AXIS {}] Updating maxMove = {}", axis, maxMove);
                }

        LOG(DEBUG, "[AXIS {}] Final move = {}, new position = {}", axis, maxMove, position[axis]);

        position[axis] += maxMove;
        if (maxMove != move)
        {
            LOG(DEBUG, "[AXIS {}] Collision occurred, velocity = 0", axis);
            velocity[axis] = 0.0;
        }
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

        auto [newPos, newVel] = sweepAABB(pos, vel, halfExtents, dt);

        transform->position = newPos;
        velocity->velocity = newVel;
        player->onGround = isOnGround(newPos, halfExtents);
    }
}

} // namespace mc::ecs
