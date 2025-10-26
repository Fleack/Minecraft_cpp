#pragma once

#include "ecs/Entity.hpp"
#include "ecs/component/ComponentList.hpp"

#include <unordered_map>

namespace mc::ecs
{

class ComponentStorage
{
public:
    template <typename T>
    void add(Entity entity, T component)
    {
        getStorage<T>()[entity] = component;
    }

    template <typename T>
    void remove(Entity entity)
    {
        getStorage<T>().erase(entity);
    }

    template <typename T>
    bool has(Entity entity) const
    {
        return getStorage<T>().contains(entity);
    }

    void removeByEntity(Entity entity)
    {
        auto removeFunc = [&]<typename... T>() {
            (remove<T>(entity), ...);
        };

        [removeFunc]<typename... T>(std::type_identity<std::tuple<T...>>) {
            removeFunc.template operator()<T...>();
        }(std::type_identity<AllComponentTypes>{});
    }

    template <typename T>
    T* get(Entity entity)
    {
        auto& storage = getStorage<T>();
        auto it = storage.find(entity);
        if (it != storage.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    template <typename T>
    std::unordered_map<Entity, T>& getAll()
    {
        return getStorage<T>();
    }

    template <typename T>
    void clear()
    {
        getStorage<T>().clear();
    }

private:
    template <typename T>
    std::unordered_map<Entity, T>& getStorage()
    {
        static std::unordered_map<Entity, T> storage;
        return storage;
    }
};
} // namespace mc::ecs
