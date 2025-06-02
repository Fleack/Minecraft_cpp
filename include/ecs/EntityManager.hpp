#pragma once

#include "Entity.hpp"
#include "component/ComponentStorage.hpp"
#include "ecs/events/EventBus.hpp"
#include "ecs/events/Events.hpp"

#include <unordered_set>

namespace mc::ecs
{

class EntityManager
{
public:
    explicit EntityManager(ComponentStorage& componentStorage, EventBus& eventBus)
        : m_componentStorage(componentStorage), m_eventBus{eventBus} {}

    Entity createEntity()
    {
        Entity entity = ++m_nextEntityId;
        m_activeEntities.insert(entity);
        m_eventBus.emit(EntityCreated{entity});
        return entity;
    }

    void destroyEntity(Entity entity)
    {
        m_activeEntities.erase(entity);
        m_componentStorage.removeByEntity(entity);
        m_eventBus.emit(EntityDestroyed{entity});
    }

    bool isAlive(Entity entity) const
    {
        return m_activeEntities.contains(entity);
    }

    std::unordered_set<Entity> const& entities() const
    {
        return m_activeEntities;
    }

private:
    uint32_t m_nextEntityId = INVALID_ENTITY;
    std::unordered_set<Entity> m_activeEntities;
    ComponentStorage& m_componentStorage;
    EventBus& m_eventBus;
};

} // namespace mc::ecs
