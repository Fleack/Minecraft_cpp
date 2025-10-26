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
        auto& vec = m_callbacks[std::type_index{typeid(EVENT)}];
        vec.emplace_back([fn = std::move(listenerCallback)](void const* p) {
            fn(*static_cast<EVENT const*>(p));
        });
    }

    template <typename EVENT>
    void emit(EVENT const& e) const
    {
        auto it = m_callbacks.find(std::type_index(typeid(EVENT)));
        if (it == m_callbacks.end()) return;

        auto local = it->second;
        for (auto const& f : local)
        {
            f(&e);
        }
    }

private:
    using ErasedCallback = std::function<void(void const*)>;
    std::unordered_map<std::type_index, std::vector<ErasedCallback>> m_callbacks;
};

} // namespace mc::ecs
