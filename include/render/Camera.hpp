#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mc::render
{
class Camera
{
public:
    [[nodiscard]] static glm::mat4 getViewMatrix(glm::vec3 const& position, glm::vec3 const& front, glm::vec3 const& up);
    [[nodiscard]] static glm::mat4 getProjectionMatrix(float fov, float aspect, float near = 0.1f, float far = 1000.0f);
};
}
