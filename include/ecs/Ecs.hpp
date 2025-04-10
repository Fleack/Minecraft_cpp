#pragma once

#include "component/ComponentStorage.hpp"
#include "ecs/Entity.hpp"
#include "ecs/system/ISystem.hpp"

#include <memory>
#include <vector>

namespace mc::ecs
{
/**
 * @brief Core ECS (Entity-Component-System) manager.
 *
 * Handles entity creation, component management, and system updates.
 */
class Ecs
{
public:
    /**
     * @brief Constructs a new ECS instance.
     */
    Ecs() : m_nextEntity(1)
    {
    }

    /**
     * @brief Creates and returns a new unique entity ID.
     *
     * @return New entity identifier.
     */
    Entity createEntity()
    {
        return m_nextEntity++;
    }

    /**
     * @brief Adds a component to the specified entity.
     *
     * @tparam T Component type.
     * @param e Entity to attach the component to.
     * @param component Component instance to add.
     */
    template <typename T>
    void addComponent(Entity e, T component)
    {
        m_components.add<T>(e, component);
    }

    /**
     * @brief Retrieves a component of the given type from an entity.
     *
     * @tparam T Component type.
     * @param e Entity to query.
     * @return Optional containing the component if present.
     */
    template <typename T>
    std::optional<T> getComponent(Entity e)
    {
        return m_components.get<T>(e);
    }

    /**
     * @brief Retrieves all components of the given type.
     *
     * @tparam T Component type.
     * @return Reference to the internal map of components.
     */
    template <typename T>
    std::unordered_map<Entity, T>& getAllComponents()
    {
        return m_components.getAll<T>();
    }

    /**
     * @brief Adds a system to the ECS.
     *
     * @param system Shared pointer to a system.
     */
    void addSystem(std::shared_ptr<ISystem> system)
    {
        m_systems.push_back(system);
    }

    /**
     * @brief Updates all systems with the given delta time.
     *
     * @param dt Time elapsed since last update.
     */
    void update(float dt)
    {
        for (auto& sys : m_systems)
        {
            sys->update(dt);
        }
    }

private:
    Entity m_nextEntity; ///< Next entity ID to assign.
    ComponentStorage m_components; ///< Storage for all components.
    std::vector<std::shared_ptr<ISystem>> m_systems; ///< List of active systems.
};
}
