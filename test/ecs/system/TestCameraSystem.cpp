#include "ecs/Ecs.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/system/CameraSystem.hpp"

#include <catch2/catch_all.hpp>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/epsilon.hpp>

using namespace mc::ecs;

class MockInputProvider final : public mc::input::InputManager
{
public:
    bool wPressed = false;
    bool aPressed = false;
    bool sPressed = false;
    bool dPressed = false;
    glm::dvec2 cursorPos = {400.0, 300.0};

    bool isKeyPressed(int key) const override
    {
        if (key == GLFW_KEY_W) return wPressed;
        if (key == GLFW_KEY_A) return aPressed;
        if (key == GLFW_KEY_S) return sPressed;
        if (key == GLFW_KEY_D) return dPressed;
        return false;
    }

    glm::dvec2 getCursorPosition() const override
    {
        return cursorPos;
    }
};

TEST_CASE("CameraSystem computes correct view/projection matrices", "[ECS][System][CameraSystem]")
{
    Ecs ecs;

    Entity cameraEntity = ecs.createEntity();
    CameraComponent camComp{
        .position = {0.0f, 0.0f, 3.0f},
        .front = {0.0f, 0.0f, -1.0f},
        .up = {0.0f, 1.0f, 0.0f},
        .fov = 45.0f
    };

    ecs.addComponent<CameraComponent>(cameraEntity, camComp);

    auto system = std::make_shared<CameraSystem>(ecs, 16.0f / 9.0f, std::make_shared<MockInputProvider>());
    ecs.addSystem(system);

    system->render();

    glm::mat4 const& view = system->getViewMatrix();
    glm::mat4 const& proj = system->getProjectionMatrix();

    glm::mat4 expectedView = glm::lookAt(camComp.position, camComp.position + camComp.front, camComp.up);

    for (int i = 0; i < 4; ++i)
    {
        REQUIRE(glm::all(glm::epsilonEqual(view[i], expectedView[i], 0.001f)));
    }

    REQUIRE(view != glm::mat4(1.0f));
    REQUIRE(proj != glm::mat4(0.0f));
}

TEST_CASE("CameraSystem syncs camera position to transform", "[ECS][System][CameraSystem]")
{
    Ecs ecs;

    Entity e = ecs.createEntity();

    CameraComponent cam;
    cam.position = {5.0f, 10.0f, -3.0f};
    cam.front = {0, 0, -1};
    cam.up = {0, 1, 0};
    cam.fov = 70.0f;

    TransformComponent transform;
    transform.position = {0.0f, 0.0f, 0.0f};

    ecs.addComponent<CameraComponent>(e, cam);
    ecs.addComponent<TransformComponent>(e, transform);

    auto mockInput = std::make_shared<MockInputProvider>();
    CameraSystem cameraSystem(ecs, 4.0f / 3.0f, mockInput);
    ecs.addSystem(std::make_shared<CameraSystem>(cameraSystem));

    mockInput->wPressed = true;
    mockInput->cursorPos = {410.0, 295.0};

    ecs.update(0.0f);

    auto& updatedTransform = ecs.getAllComponents<TransformComponent>()[e];
    REQUIRE(updatedTransform.position == cam.position);
}

TEST_CASE("CameraSystem handleInput updates position and rotation", "[ECS][System][CameraSystem]")
{
    Ecs ecs;
    Entity e = ecs.createEntity();

    CameraComponent cam;
    cam.position = {0, 0, 0};
    cam.front = {0, 0, -1};
    cam.up = {0, 1, 0};
    cam.yaw = -90.0f;
    cam.pitch = 0.0f;
    cam.speed = 5.0f;
    cam.sensitivity = 0.1f;

    ecs.addComponent<CameraComponent>(e, cam);

    auto mockInput = std::make_shared<MockInputProvider>();

    CameraSystem cameraSystem(ecs, 16.0f / 9.0f, mockInput);
    ecs.addSystem(std::make_shared<CameraSystem>(cameraSystem));
    ecs.update(0.016f); // Used for "first" inputr

    mockInput->wPressed = true;
    mockInput->cursorPos = {410.0, 295.0};
    ecs.update(0.016f);

    const auto& updatedCam = ecs.getAllComponents<CameraComponent>()[e];

    REQUIRE(updatedCam.position.z < 0.0f);
    REQUIRE(updatedCam.yaw != cam.yaw);
    REQUIRE(updatedCam.pitch != cam.pitch);
}
