#pragma once

#include "ecs/system/ISystem.hpp"

#include <memory>

namespace mc::ecs
{
class ECS;
}

namespace mc::render
{
class Shader;
}

namespace mc::ecs
{
class CameraSystem;

class RenderSystem final : public ISystem
{
public:
    RenderSystem(ECS& ecs, std::shared_ptr<CameraSystem> cameraSystem);

    void update(float deltaTime) override;

private:
    ECS& m_ecs;
    std::shared_ptr<CameraSystem> m_cameraSystem;
    std::shared_ptr<render::Shader> m_shader;
};
}
