#pragma once

namespace mc::render
{
class IShader
{
public:
    virtual ~IShader() = default;

    virtual void bind() = 0;
    virtual void unbind() = 0;

    [[nodiscard]] GLuint getID() const { return m_id; }

protected:
    GLuint m_id {0};
};
}
