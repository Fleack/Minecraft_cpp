#pragma once

namespace mc::utils
{
inline int floor_div(int a, int b)
{
    int q = a / b;
    int r = a % b;
    return (r != 0 && ((r < 0) != (b < 0))) ? q - 1 : q;
}
} // namespace mc::utils
