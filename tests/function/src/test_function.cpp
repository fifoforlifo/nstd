#define _USE_MATH_DEFINES
#include <math.h>
#include "gtest/gtest.h"
#include <functional>
#include <nstd/function.hpp>

TEST(Function, std_function)
{
}

TEST(Function, nstd_function)
{
    {
        nstd::function<int(int, int)> add2 =
            [](int x, int y)
            {
                return x + y;
            };
        int seven = add2(3, 4);
        EXPECT_EQ(7, seven);
    }
    {
        int z = 10;
        nstd::function<int(int, int)> add2 =
            [z](int x, int y)
            {
                return x + y + z;
            };
        int seven = add2(3, 4);
        EXPECT_EQ(17, seven);
    }
}
