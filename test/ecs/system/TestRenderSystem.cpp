#include "ecs/ECS.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "ecs/system/CameraSystem.hpp"
#include "render/Shader.hpp"

#include <catch2/catch_all.hpp>



using namespace mc::ecs;

class MockShader : public mc::render::Shader
{
public:
    MockShader()
        : Shader("", "")
    {
    }
    void bind() override { bound = true; }
    void unbind() override { unbound = true; }
    bool bound = false;
    bool unbound = false;
};
