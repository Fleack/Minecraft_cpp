#pragma once

#include <Magnum/GL/Attribute.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/Types.h>

namespace mc::render
{

struct Vertex
{
    Magnum::Vector3 position;
    Magnum::Vector3 normal;
    Magnum::Vector2 uv;
    Magnum::Float ao;
};

namespace attribute
{
enum : Magnum::UnsignedInt
{
    POSITION = 0,
    NORMAL = 1,
    UV = 2,
    AO = 3,
};

constexpr Magnum::GL::Attribute<POSITION, Magnum::Vector3> POSITION_ATTRIBUTE;
constexpr Magnum::GL::Attribute<NORMAL, Magnum::Vector3> NORMAL_ATTRIBUTE;
constexpr Magnum::GL::Attribute<UV, Magnum::Vector2> UV_ATTRIBUTE;
constexpr Magnum::GL::Attribute<AO, Magnum::Float> AO_ATTRIBUTE;
} // namespace attribute

} // namespace mc::render
