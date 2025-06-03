#pragma once

namespace mc::ecs
{
enum Type
{
    CAMERA,
    CAMERA_INPUT,
    CHUNK_LOADING,
    COLLISION,
    GRAVITY,
    JUMP,
    MOVEMENT,
    PLAYER_INPUT,
    RENDER,
    UI
};

class ISystem
{
public:
    explicit ISystem(Type type)
        : m_type(type) {}
    virtual void update(float deltaTime) = 0;
    virtual void render(float deltaTime) = 0;
    virtual ~ISystem() = default;

private:
    Type m_type;
};
} // namespace mc::ecs
