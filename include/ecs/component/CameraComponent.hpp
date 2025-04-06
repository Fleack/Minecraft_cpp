#pragma once

#include <glm/glm.hpp>

namespace mc::ecs
{
struct CameraComponent
{
    glm::vec3 position{0.0f, 100.0f, 0.0f};
    glm::vec3 front{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};

    float yaw = -90.0f;
    float pitch = 0.0f;
    float fov = 70.0f;

    float speed = 10.0f;
    float sensitivity = 0.1f;
};
}
