#pragma once

#include <ecs/Ecs.hpp>
#include <ecs/system/ISystem.hpp>

namespace mc::ecs
{

class MovementSystem final : public ISystem
{
public:
    explicit MovementSystem(Ecs& ecs);

    void update(float deltaTime) override;

private:
    Ecs& m_ecs; ///< Reference to the ECS manager.
};
} // namespace mc::ecs
