#define _USE_MATH_DEFINES
#include <math.h>
#include "gtest/gtest.h"
#include <functional>
#include <nstd/function.hpp>

TEST(Function, std_function)
{
    {
        std::function<double(double)> dd = (double(*)(double))&fabs;
        dd = std::function<double(double)>([](double) {return 1.0;});
        dd(1.0 * M_PI / 2);
    }
#if !(__clang_major__ && _WIN32)
    {
        std::function<double(double)> dd = (double(*)(double))&sin;
        dd(1.0 * M_PI / 2);
        dd = [](double x) { return sin(x); };
        double y = M_PI * 2;
        dd = [&y](double x) { return sin(x + y); };
        dd(1.0 * M_PI / 2);
    }
    {
        double z = 10;
        std::function<double(double, double)> add2 =
            [z](double x, double y)
            {
                return x + y + z;
            };
        if (!add2)
        {
            add2 = [](double x, double y) { return x + y; };
        }
        double seven = add2(3, 4);
        EXPECT_EQ(17.0, seven);
    }
#endif
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
        double z = 10;
        nstd::function<double(double, double)> add2 =
            [z](double x, double y)
            {
                return x + y + z;
            };
        if (!add2)
        {
            add2 = [](double x, double y) { return x + y; };
        }
        double seven = add2(3, 4);
        EXPECT_EQ(17.0, seven);
    }
}
