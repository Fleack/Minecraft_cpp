#pragma once

#include <Magnum/Math/Angle.h>
#include <Magnum/Math/Quaternion.h>
#include <Magnum/Math/Matrix4.h>

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
    float sensitivity{0.045f};
    float fov{90.0f};

    Magnum::Math::Deg<float> yaw{-90.0f};
    Magnum::Math::Deg<float> pitch{0.0f};

    Magnum::Quaternion orientation{};
    Magnum::Matrix4 viewMatrix{Magnum::Math::IdentityInit};
    Magnum::Matrix4 projectionMatrix{Magnum::Math::IdentityInit};
    Magnum::Matrix4 rotationMatrix{Magnum::Math::IdentityInit};
    bool active{true};
};
} // namespace mc::ecs
