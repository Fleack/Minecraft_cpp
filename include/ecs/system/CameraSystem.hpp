#pragma once

#include "ecs/Ecs.hpp"

#include <Magnum/Math/Matrix4.h>
#include <optional>

namespace mc::ecs
{


class CameraSystem final : public ISystem
{
public:
    CameraSystem(Ecs& ecs, float aspectRatio);

    void update(float dt) override;
    void render(float) override {}

    Magnum::Matrix4 const& getViewMatrix() const;
    Magnum::Matrix4 const& getProjectionMatrix() const;
    Magnum::Matrix4 const& getRotationMatrix() const;

    void setAspectRatio(float ar);

private:
    Ecs& m_ecs;
    float m_aspectRatio;

    Magnum::Matrix4 m_viewMatrix{Magnum::Math::IdentityInit};
    Magnum::Matrix4 m_projectionMatrix{Magnum::Math::IdentityInit};
    Magnum::Matrix4 m_rotationMatrix{Magnum::Math::IdentityInit};
    std::optional<Entity> m_activeCamera;
};

} // namespace mc::ecs
