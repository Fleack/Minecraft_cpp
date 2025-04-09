#pragma once

#include "component/ComponentStorage.hpp"
#include "ecs/Entity.hpp"
#include "ecs/system/ISystem.hpp"

#include <memory>
#include <vector>

namespace mc::ecs
{
class Ecs
{
public:
    Ecs() : m_nextEntity(1)
    {
    }

    Entity createEntity()
    {
        return m_nextEntity++;
    }

    template <typename T>
    void addComponent(Entity e, T component)
    {
        m_components.Add<T>(e, component);
    }

    template <typename T>
    std::optional<T> getComponent(Entity e)
    {
        return m_components.Get<T>(e);
    }

    template <typename T>
    std::unordered_map<Entity, T>& getAllComponents()
    {
        return m_components.GetAll<T>();
    }

    void addSystem(std::shared_ptr<ISystem> system)
    {
        m_systems.push_back(system);
    }

    void update(float dt)
    {
        for (auto& sys : m_systems)
        {
            sys->update(dt);
        }
    }

private:
    Entity m_nextEntity;
    ComponentStorage m_components;
    std::vector<std::shared_ptr<ISystem>> m_systems;
};
}
