#pragma once

#include <functional>
#include <optional>
#include <queue>
#include <unordered_set>
#include <vector>

namespace mc::utils
{

template <typename T, typename HASH = std::hash<T>, typename COMPARE = std::greater<T>>
class PriorityUniqueQueue
{
public:
    void push(T const& value)
    {
        if (m_set.insert(value).second)
        {
            m_queue.emplace(value);
        }
    }

    bool contains(T const& val)
    {
        return m_set.contains(val);
    }

    std::optional<T> pop()
    {
        while (!m_queue.empty())
        {
            auto const val = m_queue.top();
            m_queue.pop();
            if (m_set.erase(val)) return val;
        }
        return std::nullopt;
    }

    bool empty() const
    {
        return m_set.empty();
    }

    void clear()
    {
        m_set.clear();
        m_queue = {};
    }

private:
    std::unordered_set<T, HASH> m_set;
    std::priority_queue<T, std::vector<T>, COMPARE> m_queue;
};

} // namespace mc::utils
