#include "render/Camera.hpp"

namespace mc::render
{
glm::mat4 Camera::getViewMatrix(glm::vec3 const& position, glm::vec3 const& front, glm::vec3 const& up)
{
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float fov, float aspect, float near, float far)
{
    return glm::perspective(glm::radians(fov), aspect, near, far);
}
}
