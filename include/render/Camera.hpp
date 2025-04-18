#pragma once

#include <glm/gtc/matrix_transform.hpp>

namespace mc::render
{
/**
 * @brief Utility class for camera-related matrix calculations.
 *
 * Provides static methods to generate view and projection matrices
 * used in 3D rendering.
 */
class Camera
{
public:
    /**
     * @brief Calculates the view matrix using camera position and orientation.
     *
     * @param position Camera world position.
     * @param front Direction the camera is facing.
     * @param up Up vector for the camera.
     * @return View matrix (look-at matrix).
     */
    [[nodiscard]] static glm::mat4 getViewMatrix(glm::vec3 const& position, glm::vec3 const& front, glm::vec3 const& up);

    /**
     * @brief Generates a perspective projection matrix.
     *
     * @param fov Field of view in degrees.
     * @param aspect Aspect ratio (width / height).
     * @param near Near clipping plane.
     * @param far Far clipping plane.
     * @return Perspective projection matrix.
     */
    [[nodiscard]] static glm::mat4 getProjectionMatrix(float fov, float aspect, float near = 0.1f, float far = 1000.0f);
};
} // namespace mc::render
