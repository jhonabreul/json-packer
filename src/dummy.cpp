#include <algorithm>

#include "dummy.hpp"

auto fib(int x) noexcept -> int
{
    int a = 0, b = 1;

    for (int i = 0; i < x; ++i) {
        a = std::exchange(b, a + b);
    }

    return a;
}
