#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Shaders/GenericGL.h>

namespace mc::render
{

class ShaderProgram final : public Magnum::GL::AbstractShaderProgram
{
public:
    explicit ShaderProgram();

    ShaderProgram& setViewMatrix(Magnum::Matrix4 const& matrix);
    ShaderProgram& setProjectionMatrix(Magnum::Matrix4 const& matrix);
    ShaderProgram& setModelMatrix(Magnum::Matrix4 const& matrix);
    ShaderProgram& bindTexture(Magnum::GL::Texture2D& texture);

private:
    Magnum::Int m_uView{};
    Magnum::Int m_uProjection{};
    Magnum::Int m_uModel{};
    Magnum::Int m_uTexture{};
};

} // namespace mc::render
