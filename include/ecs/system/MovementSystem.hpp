#pragma once

#include "ISystem.hpp"

namespace mc::ecs
{
class Ecs;

/**
 * @brief Simple ECS system that moves entities over time.
 *
 * Currently, applies a constant movement along the X-axis
 * to all entities that have a TransformComponent.
 */
class MovementSystem final : public ISystem
{
public:
    /**
     * @brief Constructs the movement system.
     *
     * @param ecs Reference to the ECS manager.
     */
    explicit MovementSystem(Ecs& ecs);

    /**
     * @brief Updates all transform components.
     *
     * Moves each entity along the X-axis at a fixed speed.
     *
     * @param deltaTime Time step used for smooth movement.
     */
    void update(float deltaTime) override;

    /**
     * @brief Does nothing.
     */
    void render() override {};

private:
    Ecs& m_ecs; ///< Reference to the ECS manager.
};
}
