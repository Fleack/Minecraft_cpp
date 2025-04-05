#include "ecs/system/CameraSystem.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "render/Camera.hpp"

#include <algorithm>

#include <GLFW/glfw3.h>

#include "ecs/component/TransformComponent.hpp"

namespace mc::ecs
{
CameraSystem::CameraSystem(ECS& ecs, float aspectRatio)
    : m_ecs(ecs), m_aspectRatio(aspectRatio)
{
    updateMatrices();
}

void CameraSystem::update(float dt)
{
    handleInput(dt);
    updateMatrices();

    auto& cameras = m_ecs.getAllComponents<CameraComponent>();
    auto& transforms = m_ecs.getAllComponents<TransformComponent>();

    if (!cameras.empty() && !transforms.empty())
    {
        auto& camera = cameras.begin()->second;
        auto& transform = transforms.begin()->second;
        transform.position = camera.position;
    }
}

void CameraSystem::handleInput(float dt)
{
    auto& cams = m_ecs.getAllComponents<CameraComponent>();
    if (cams.empty()) return;

    CameraComponent& cam = cams.begin()->second;
    GLFWwindow* window = glfwGetCurrentContext();
    if (!window) return;

    float velocity = cam.speed * dt;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.position += cam.front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.position -= cam.front * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.position -= glm::normalize(glm::cross(cam.front, cam.up)) * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.position += glm::normalize(glm::cross(cam.front, cam.up)) * velocity;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (m_firstMouse)
    {
        m_lastX = static_cast<float>(xpos);
        m_lastY = static_cast<float>(ypos);
        m_firstMouse = false;
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

void CameraSystem::updateMatrices()
{
    auto& cams = m_ecs.getAllComponents<CameraComponent>();
    if (cams.empty()) { return; }

    CameraComponent const& cam = cams.begin()->second;

    m_viewMatrix = render::Camera::getViewMatrix(cam.position, cam.front, cam.up);
    m_projectionMatrix = render::Camera::getProjectionMatrix(cam.fov, m_aspectRatio);
}

const glm::mat4& CameraSystem::getViewMatrix() const
{
    return m_viewMatrix;
}

const glm::mat4& CameraSystem::getProjectionMatrix() const
{
    return m_projectionMatrix;
}
}
