#pragma once

#include <cstdint>

namespace mc::ecs
{
using Entity = std::uint32_t;
constexpr Entity INVALID_ENTITY = 0;
} // namespace mc::ecs
