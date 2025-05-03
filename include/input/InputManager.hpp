#pragma once

#include <mutex>
#include <unordered_set>

#include <Magnum/Math/Vector2.h>
#include <Magnum/Platform/Sdl2Application.h>

namespace mc::input
{

class InputManager
{
public:
    using Key = Magnum::Platform::Sdl2Application::Key;

    InputManager() = default;

    void keyPressEvent(Magnum::Platform::Sdl2Application::KeyEvent const& event)
    {
        std::lock_guard lock(m_mutex);
        m_pressedKeys.insert(event.key());
    }

    void keyReleaseEvent(Magnum::Platform::Sdl2Application::KeyEvent const& event)
    {
        std::lock_guard lock(m_mutex);
        m_pressedKeys.erase(event.key());
    }

    void pointerMoveEvent(Magnum::Platform::Sdl2Application::PointerMoveEvent const& event)
    {
        std::lock_guard lock(m_mutex);
        auto const p = event.position();
        m_cursorPos = {p.x(), p.y()};
    }

    void scrollEvent(Magnum::Platform::Sdl2Application::ScrollEvent const& event)
    {
        std::lock_guard lock(m_mutex);
        m_scrollY += event.offset().y();
    }

    void update()
    {
        std::lock_guard lock(m_mutex);
        m_scrollY = 0.0f;
    }

    bool isKeyPressed(Key key) const
    {
        std::lock_guard lock(m_mutex);
        return m_pressedKeys.contains(key);
    }

    Magnum::Math::Vector2<double> cursorPosition() const
    {
        std::lock_guard lock(m_mutex);
        return m_cursorPos;
    }

    float scrollDelta() const
    {
        std::lock_guard lock(m_mutex);
        return m_scrollY;
    }

private:
    mutable std::mutex m_mutex;
    std::unordered_set<Key> m_pressedKeys;
    Magnum::Math::Vector2<double> m_cursorPos{0.0, 0.0};
    float m_scrollY{0.0f};
};

} // namespace mc::input
