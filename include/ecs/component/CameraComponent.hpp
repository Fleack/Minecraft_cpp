#pragma once

#include <glm/glm.hpp>

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
    glm::vec3 position{0.0f, 100.0f, 0.0f}; ///< World position of the camera.
    glm::vec3 front{0.0f, 0.0f, -1.0f}; ///< Direction the camera is facing.
    glm::vec3 up{0.0f, 1.0f, 0.0f}; ///< Up vector used to define the camera orientation.

    float yaw = -90.0f; ///< Yaw angle in degrees (rotation around the Y-axis).
    float pitch = 0.0f; ///< Yaw angle in degrees (rotation around the Y-axis).
    float fov = 70.0f; ///< Field of view angle in degrees.

    float speed = 10.0f; ///< Movement speed of the camera.
    float sensitivity = 0.1f; ///< Mouse sensitivity for controlling the camera.
};
}
