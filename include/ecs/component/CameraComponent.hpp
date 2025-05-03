#pragma once

namespace mc::ecs
{
/**
 * @brief Represents a camera component in the ECS.
 *
 * Stores the position, orientation, and movement parameters of a camera
 * used for rendering and view transformation in a 3D environment.
 */
struct CameraComponent
{
    float speed{10.0f};
    float sensitivity{0.05f};
    float fov{70.0f};

    Magnum::Math::Deg<float> yaw{-90.0f};
    Magnum::Math::Deg<float> pitch{0.0f};
};
} // namespace mc::ecs
