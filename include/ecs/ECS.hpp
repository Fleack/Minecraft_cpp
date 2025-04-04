#pragma once

#include "component/ComponentStorage.hpp"
#include "ecs/Entity.hpp"
#include "ecs/system/ISystem.hpp"

#include <memory>
#include <vector>

namespace mc::ecs
{
class ECS
{
public:
    ECS() : m_nextEntity(1)
    {
    }

    Entity CreateEntity()
    {
        return m_nextEntity++;
    }

    template <typename T>
    void AddComponent(Entity e, T component)
    {
        m_components.Add<T>(e, component);
    }

    template <typename T>
    T* GetComponent(Entity e)
    {
        return m_components.Get<T>(e);
    }

    template <typename T>
    std::unordered_map<Entity, T>& GetAllComponents()
    {
        return m_components.GetAll<T>();
    }

    void AddSystem(std::shared_ptr<ISystem> system)
    {
        m_systems.push_back(system);
    }

    void Update(float dt)
    {
        for (auto& sys : m_systems)
        {
            sys->Update(dt);
        }
    }

private:
    Entity m_nextEntity;
    ComponentStorage m_components;
    std::vector<std::shared_ptr<ISystem>> m_systems;
};
}
