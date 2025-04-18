#include "ecs/system/CameraSystem.hpp"
#include "core/Logger.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "render/Camera.hpp"

#include <algorithm>

#include <GLFW/glfw3.h>

#include "ecs/component/TransformComponent.hpp"

namespace mc::ecs
{
CameraSystem::CameraSystem(Ecs& ecs, float aspectRatio, std::shared_ptr<input::IInputProvider> inputProvider)
    : m_ecs(ecs), m_input(std::move(inputProvider)), m_aspectRatio(aspectRatio)
{
    LOG(INFO, "CameraSystem initialized with aspect ratio: {}", aspectRatio);
}

void CameraSystem::update(float dt)
{
    handleInput(dt);

    auto& cameras = m_ecs.getAllComponents<CameraComponent>();
    auto& transforms = m_ecs.getAllComponents<TransformComponent>();

    if (!cameras.empty() && !transforms.empty())
    {
        auto& camera = cameras.begin()->second;
        auto& transform = transforms.begin()->second;
        transform.position = camera.position;
    }
}

void CameraSystem::render()
{
    auto& cams = m_ecs.getAllComponents<CameraComponent>();
    if (cams.empty())
    {
        LOG(WARN, "No camera components found during render");
        return;
    }

    const auto& cam = cams.begin()->second;

    m_viewMatrix = render::Camera::getViewMatrix(cam.position, cam.front, cam.up);
    m_projectionMatrix = render::Camera::getProjectionMatrix(cam.fov, m_aspectRatio);
}

void CameraSystem::handleInput(float dt)
{
    auto& cams = m_ecs.getAllComponents<CameraComponent>();
    if (cams.empty())
    {
        LOG(WARN, "No camera component found for input handling");
        return;
    }

    CameraComponent& cam = cams.begin()->second;

    float velocity = cam.speed * dt;

    if (m_input->isKeyPressed(GLFW_KEY_LEFT_SHIFT))
        velocity *= 2.5f;
    if (m_input->isKeyPressed(GLFW_KEY_W))
        cam.position += cam.front * velocity;
    if (m_input->isKeyPressed(GLFW_KEY_S))
        cam.position -= cam.front * velocity;
    if (m_input->isKeyPressed(GLFW_KEY_A))
        cam.position -= glm::normalize(glm::cross(cam.front, cam.up)) * velocity;
    if (m_input->isKeyPressed(GLFW_KEY_D))
        cam.position += glm::normalize(glm::cross(cam.front, cam.up)) * velocity;
    if (m_input->isKeyPressed(GLFW_KEY_SPACE))
        cam.position += cam.up * velocity;
    if (m_input->isKeyPressed(GLFW_KEY_LEFT_CONTROL))
        cam.position -= cam.up * velocity;

    glm::dvec2 cursor = m_input->getCursorPosition();
    double xpos = cursor.x;
    double ypos = cursor.y;

    if (m_firstMouseInput)
    {
        m_lastX = static_cast<float>(xpos);
        m_lastY = static_cast<float>(ypos);
        m_firstMouseInput = false;
    }

    float xoffset = static_cast<float>(xpos) - m_lastX;
    float yoffset = m_lastY - static_cast<float>(ypos); // reversed

    m_lastX = static_cast<float>(xpos);
    m_lastY = static_cast<float>(ypos);

    xoffset *= cam.sensitivity;
    yoffset *= cam.sensitivity;

    cam.yaw += xoffset;
    cam.pitch += yoffset;

    cam.pitch = std::clamp(cam.pitch, -89.0f, 89.0f);

    glm::vec3 front;
    front.x = cos(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
    front.y = sin(glm::radians(cam.pitch));
    front.z = sin(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
    cam.front = glm::normalize(front);
}

const glm::mat4& CameraSystem::getViewMatrix() const
{
    return m_viewMatrix;
}

const glm::mat4& CameraSystem::getProjectionMatrix() const
{
    return m_projectionMatrix;
}
} // namespace mc::ecs
