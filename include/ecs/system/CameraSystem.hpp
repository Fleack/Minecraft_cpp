#pragma once

#include "ecs/Ecs.hpp"
#include "ecs/system/ISystem.hpp"
#include "input/IInputProvider.hpp"

#include "glm/mat4x4.hpp"

namespace mc::ecs
{
/**
 * @brief ECS system for controlling and updating the camera.
 *
 * Handles user input (keyboard + mouse) to move and rotate the camera,
 * updates transform components accordingly, and computes view/projection matrices
 * for use in rendering.
 */
class CameraSystem final : public ISystem
{
public:
    /**
     * @brief Constructs the camera system.
     *
     * @param ecs Reference to the ECS registry.
     * @param aspectRatio Initial screen aspect ratio (width / height).
     * @param inputProvider Shared input provider for handling movement and mouse.
     */
    explicit CameraSystem(Ecs& ecs, float aspectRatio, std::shared_ptr<input::IInputProvider> inputProvider);

    /**
     * @brief Updates the camera based on input and applies movement.
     *
     * Also synchronizes the camera's world position with its associated transform component.
     *
     * @param dt Delta time since the last frame.
     */
    void update(float deltaTime) override;

    /**
     * @brief Computes the view and projection matrices from the current camera state.
     *
     * Should be called once per frame before rendering.
     */
    void render() override;

    [[nodiscard]] glm::mat4 const& getViewMatrix() const;
    [[nodiscard]] glm::mat4 const& getProjectionMatrix() const;

private:
    /**
     * @brief Handles WASD movement and mouse-based camera rotation.
     *
     * Updates the camera component's position and orientation accordingly.
     *
     * @param dt Time step used to scale movement speed.
     */
    void handleInput(float dt);

private:
    Ecs& m_ecs; ///< Reference to the ECS manager.
    std::shared_ptr<input::IInputProvider> m_input; ///< Input abstraction for key/mouse.

    glm::mat4 m_viewMatrix{}; ///< Cached view matrix.
    glm::mat4 m_projectionMatrix{}; ///< Cached projection matrix.

    bool m_firstMouseInput = true; ///< Tracks whether the first mouse frame has been handled.
    float m_aspectRatio = 16.0f / 9.0f; ///< Aspect ratio used for projection matrix.
    float m_lastX = 640.0f; ///< Last known mouse X position.
    float m_lastY = 360.0f; ///< Last known mouse Y position.
};
}
