#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Texture.h>

namespace mc::render
{

class ShaderProgram final : public Magnum::GL::AbstractShaderProgram
{
public:
    explicit ShaderProgram();

    // Basic render
    ShaderProgram& setViewMatrix(Magnum::Matrix4 const& matrix);
    ShaderProgram& setProjectionMatrix(Magnum::Matrix4 const& matrix);
    ShaderProgram& setModelMatrix(Magnum::Matrix4 const& matrix);

    // Texture
    ShaderProgram& bindTexture(Magnum::GL::Texture2D& texture);

    // Lightning
    ShaderProgram& setLightDirection(Magnum::Vector3 const& dir);
    ShaderProgram& setLightColor(Magnum::Vector3 const& color);
    ShaderProgram& setAmbientColor(Magnum::Vector3 const& color);

    // Fog
    ShaderProgram& setCameraPosition(Magnum::Vector3 const& pos);
    ShaderProgram& setFogColor(Magnum::Vector3 const& color);
    ShaderProgram& setFogDistance(float distance);

private:
    // Basic render
    Magnum::Int m_uView{};
    Magnum::Int m_uProjection{};
    Magnum::Int m_uModel{};

    // Texture
    Magnum::Int m_uTexture{};

    // Lighting
    Magnum::Int m_uLightDir{};
    Magnum::Int m_uLightColor{};
    Magnum::Int m_uAmbientColor{};

    // Fog
    Magnum::Int m_uCameraPos{};
    Magnum::Int m_uFogColor{};
    Magnum::Int m_uFogDistance{};
};

} // namespace mc::render
