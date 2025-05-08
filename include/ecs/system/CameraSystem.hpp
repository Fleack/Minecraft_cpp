#pragma once

#include "ecs/Ecs.hpp"

#include <memory>
#include <set>

#include <Magnum/Math/Matrix4.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Scene.h>

namespace mc::ecs
{

using Scene3D = Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D>;
using Object3D = Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>;
using Camera3D = Magnum::SceneGraph::Camera3D;

class CameraSystem final : public ISystem
{
public:
    CameraSystem(Ecs& ecs, float aspectRatio);

    void update(float dt) override;
    void render(float) override;

    void handleMouse(Magnum::Vector2 const& delta);
    void handleScroll(float yOffset);
    void handleKey(Magnum::Platform::Sdl2Application::Key key, bool pressed);

    Magnum::Math::Matrix4<float> const& getViewMatrix() const;
    Magnum::Math::Matrix4<float> const& getProjectionMatrix() const;

    void setAspectRatio(float ar);

private:
    Ecs& m_ecs;
    float m_aspectRatio;

    Scene3D m_scene;
    std::unique_ptr<Object3D> m_cameraObject;
    std::unique_ptr<Camera3D> m_camera;

    std::set<Magnum::Platform::Sdl2Application::Key> m_keysPressed;

    Magnum::Math::Matrix4<float> m_view;
    Magnum::Math::Matrix4<float> m_proj;
};

} // namespace mc::ecs
