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

    bindFragmentDataLocation(0, "FragColor");

    if (!link())
    {
        CORRADE_ASSERT(false, "Shader link failed", );
    }

    m_uView = uniformLocation("u_View");
    m_uProjection = uniformLocation("u_Projection");
    m_uModel = uniformLocation("u_Model");
    m_uTexture = uniformLocation("u_Texture");
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

ShaderProgram& ShaderProgram::bindTexture(GL::Texture2D& texture)
{
    texture.bind(0);
    setUniform(m_uTexture, 0);
    return *this;
}

} // namespace mc::render
