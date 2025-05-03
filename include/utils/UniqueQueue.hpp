#pragma once

#include <vector>

#include <tsl/hopscotch_set.h>

namespace mc::utils
{
template <typename T, class HASH = std::hash<T>>
class UniqueQueue
{
public:
    bool enqueue(T const& value)
    {
        if (m_set.insert(value).second)
        {
            m_queue.push_back(value);
            return true;
        }
        return false;
    }

    bool empty() const
    {
        return m_index >= m_queue.size();
    }

    T const& front() const
    {
        return m_queue[m_index];
    }

    void pop()
    {
        ++m_index;
    }

    void clear()
    {
        m_queue.clear();
        m_set.clear();
        m_index = 0;
    }

private:
    std::vector<T> m_queue;
    tsl::hopscotch_set<T, HASH> m_set;
    size_t m_index = 0;
};
} // namespace mc::utils
