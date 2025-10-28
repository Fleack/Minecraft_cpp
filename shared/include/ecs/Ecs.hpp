#pragma once

#include "../../../client/include/systems/IRenderableSystem.hpp"
#include "component/ComponentStorage.hpp"
#include "ecs/EntityManager.hpp"
#include "ecs/events/EventBus.hpp"
#include "ecs/system/ISystem.hpp"

#include <memory>
#include <vector>

namespace mc::ecs
{

class Ecs
{
public:
    Ecs()
        : m_entityManager(m_components, m_eventBus) {}

    Entity createEntity()
    {
        return m_entityManager.createEntity();
    }

    void destroyEntity(Entity e)
    {
        m_entityManager.destroyEntity(e);
    }

    bool isAlive(Entity e) const
    {
        return m_entityManager.isAlive(e);
    }

    std::unordered_set<Entity> const& entities() const
    {
        return m_entityManager.entities();
    }

    template <typename T>
    void addComponent(Entity e, T component)
    {
        m_components.add<T>(e, component);
    }

    template <typename T>
    T* getComponent(Entity e)
    {
        return m_components.get<T>(e);
    }

    template <typename T>
    bool hasComponent(Entity e) const
    {
        return m_components.has<T>(e);
    }

    template <typename T>
    void removeComponent(Entity e)
    {
        m_components.remove<T>(e);
    }

    template <typename T>
    std::unordered_map<Entity, T>& getAllComponents()
    {
        return m_components.getAll<T>();
    }

    EventBus& eventBus()
    {
        return m_eventBus;
    }

    EventBus const& eventBus() const
    {
        return m_eventBus;
    }

    void addSystem(std::shared_ptr<ISystem> system)
    {
        m_systems.push_back(system);
    }

    void addSystem(std::shared_ptr<IRenderableSystem> system)
    {
        addSystem(static_cast<std::shared_ptr<ISystem>>(system));
        m_renderableSystems.push_back(system);
    }

    void update(float dt)
    {
        for (auto& sys : m_systems)
        {
            sys->update(dt);
        }
    }

    void render(float dt)
    {
        for (auto& sys : m_renderableSystems)
        {
            sys->render(dt);
        }
    }

private:
    ComponentStorage m_components;
    EntityManager m_entityManager;
    std::vector<std::shared_ptr<ISystem>> m_systems;
    std::vector<std::shared_ptr<IRenderableSystem>> m_renderableSystems;
    EventBus m_eventBus;
};

} // namespace mc::ecs
