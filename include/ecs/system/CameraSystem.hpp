#pragma once

#include "ecs/Ecs.hpp"
#include "ecs/system/ISystem.hpp"
#include "input/IInputProvider.hpp"

#include "glm/mat4x4.hpp"

namespace mc::ecs
{
class CameraSystem final : public ISystem
{
public:
    explicit CameraSystem(Ecs& ecs, float aspectRatio, std::shared_ptr<input::IInputProvider> inputProvider);

    void update(float deltaTime) override;

    [[nodiscard]] glm::mat4 const& getViewMatrix() const;
    [[nodiscard]] glm::mat4 const& getProjectionMatrix() const;

private:
    void handleInput(float dt);
    void updateMatrices();

private:
    std::shared_ptr<input::IInputProvider> m_input;

    Ecs& m_ecs;
    float m_aspectRatio = 16.0f / 9.0f;

    glm::mat4 m_viewMatrix{};
    glm::mat4 m_projectionMatrix{};

    bool m_firstMouseInput = true;
    float m_lastX = 640.0f;
    float m_lastY = 360.0f;
};
}
