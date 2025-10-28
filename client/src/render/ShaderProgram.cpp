#include "render/ShaderProgram.hpp"

#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Matrix4.h>

namespace mc::render
{

using namespace Magnum;

ShaderProgram::ShaderProgram()
{
    GL::Shader vertexShader{GL::Version::GL460, GL::Shader::Type::Vertex};
    GL::Shader fragmentShader{GL::Version::GL460, GL::Shader::Type::Fragment};

    vertexShader.addFile("shaders/voxel.vert");
    fragmentShader.addFile("shaders/voxel.frag");

    CORRADE_INTERNAL_ASSERT_OUTPUT(vertexShader.compile());
    CORRADE_INTERNAL_ASSERT_OUTPUT(fragmentShader.compile());

    attachShaders({vertexShader, fragmentShader});

    bindAttributeLocation(0, "a_Position");
    bindAttributeLocation(1, "a_Normal");
    bindAttributeLocation(2, "a_UV");
    bindAttributeLocation(3, "a_AO");

    bindFragmentDataLocation(0, "FragColor");

    if (!link())
    {
        CORRADE_ASSERT(false, "Shader link failed", );
    }

    // Basic render
    m_uView = uniformLocation("u_View");
    m_uProjection = uniformLocation("u_Projection");
    m_uModel = uniformLocation("u_Model");

    // Texture
    m_uTexture = uniformLocation("u_Texture");

    // Lightning
    m_uLightDir = uniformLocation("u_LightDir");
    m_uLightColor = uniformLocation("u_LightColor");
    m_uAmbientColor = uniformLocation("u_AmbientColor");

    // Fog
    m_uCameraPos = uniformLocation("u_CameraPos");
    m_uFogColor = uniformLocation("u_FogColor");
    m_uFogDistance = uniformLocation("u_FogDistance");
}

ShaderProgram& ShaderProgram::setViewMatrix(Matrix4 const& matrix)
{
    setUniform(m_uView, matrix);
    return *this;
}

ShaderProgram& ShaderProgram::setProjectionMatrix(Matrix4 const& matrix)
{
    setUniform(m_uProjection, matrix);
    return *this;
}

ShaderProgram& ShaderProgram::setModelMatrix(Matrix4 const& matrix)
{
    setUniform(m_uModel, matrix);
    return *this;
}

ShaderProgram& ShaderProgram::setLightDirection(Vector3 const& dir)
{
    setUniform(m_uLightDir, dir);
    return *this;
}

ShaderProgram& ShaderProgram::setLightColor(Vector3 const& color)
{
    setUniform(m_uLightColor, color);
    return *this;
}

ShaderProgram& ShaderProgram::setAmbientColor(Vector3 const& color)
{
    setUniform(m_uAmbientColor, color);
    return *this;
}

ShaderProgram& ShaderProgram::setCameraPosition(Vector3 const& pos)
{
    setUniform(m_uCameraPos, pos);
    return *this;
}

ShaderProgram& ShaderProgram::setFogColor(Magnum::Vector3 const& color)
{
    setUniform(m_uFogColor, color);
    return *this;
}

ShaderProgram& ShaderProgram::setFogDistance(float distance)
{
    setUniform(m_uFogDistance, distance);
    return *this;
}

ShaderProgram& ShaderProgram::bindTexture(GL::Texture2D& texture)
{
    texture.bind(0);
    setUniform(m_uTexture, 0);
    return *this;
}

} // namespace mc::render
