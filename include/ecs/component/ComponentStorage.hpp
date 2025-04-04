#pragma once

#include <optional>

#include "ecs/Entity.hpp"

#include <unordered_map>

namespace mc::ecs
{
class ComponentStorage
{
public:
    template <typename T>
    void Add(Entity entity, T component)
    {
        GetStorage<T>()[entity] = component;
    }

    template <typename T>
    std::optional<T> Get(Entity entity)
    {
        auto& storage = GetStorage<T>();
        auto it = storage.find(entity);
        if (it != storage.end())
        {
            return it->second;
        }
        return std::nullopt;
    }

    template <typename T>
    std::unordered_map<Entity, T>& GetAll()
    {
        return GetStorage<T>();
    }

private:
    template <typename T>
    std::unordered_map<Entity, T>& GetStorage()
    {
        static std::unordered_map<Entity, T> storage;
        return storage;
    }
};
}
