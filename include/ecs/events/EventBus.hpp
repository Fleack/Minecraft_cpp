#pragma once

#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace mc::ecs
{

class EventBus
{
public:
    template <typename EVENT>
    using Listener = std::function<void(EVENT const&)>;

    template <typename EVENT>
    void subscribe(Listener<EVENT> listenerCallback)
    {
        auto& listeners = getListeners<EVENT>();
        listeners.push_back(std::move(listenerCallback));
    }

    template <typename EVENT>
    void emit(EVENT const& event) const
    {
        auto it = m_listeners.find(std::type_index(typeid(EVENT)));
        if (it == m_listeners.end()) return;

        auto const& baseList = it->second;
        for (auto const& baseFn : *baseList)
        {
            auto* fn = static_cast<Listener<EVENT> const*>(baseFn.get());
            (*fn)(event);
        }
    }

private:
    template <typename EVENT>
    std::vector<std::shared_ptr<void>>& getListeners()
    {
        auto type = std::type_index(typeid(EVENT));
        auto& baseList = m_listeners[type];
        if (!baseList)
            baseList = std::make_shared<BaseListenerList>();

        return *baseList;
    }

private:
    using BaseListenerList = std::vector<std::shared_ptr<void>>;
    std::unordered_map<std::type_index, std::shared_ptr<BaseListenerList>> m_listeners;
};

} // namespace mc::ecs
