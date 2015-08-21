#define _USE_MATH_DEFINES
#include <math.h>
#include "gtest/gtest.h"
#include <functional>
#include <nstd/function.hpp>

struct Heavy
{
    std::vector<double> values;
    std::vector<double> values2;

    double method(double x, double y, double z, double w)
    {
        double acc = 0;
        for (auto value : values)
        {
            acc += value * (x + y + z + w);
        }
        for (auto value : values2)
        {
            acc += value * (x + y + z + w);
        }
        return acc;
    }
};

TEST(Function, std_function)
{
    {
        std::function<double(double)> dd = (double(*)(double))&fabs;
        dd = std::function<double(double)>([](double) {return 1.0;});
        dd(1.0 * M_PI / 2);
    }
#if !(__clang_major__ && _WIN32)
    {
        std::function<int(
            int, int, int, int,
            int, int, int, int,
            int, int, int, int,
            int, int, int, int,
            int, int, int, int,
            int, int, int, int,
            int, int, int, int,
            int, int, int, int)> fn =
            [](
                int, int, int, int,
                int, int, int, int,
                int, int, int, int,
                int, int, int, int,
                int, int, int, int,
                int, int, int, int,
                int, int, int, int,
                int, int, int, int)
            {
                return 3;
            };
        if (!(new (std::nothrow) char('A')))
        {
            fn = nullptr;
        }
        int x = fn(1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8);
        EXPECT_EQ(3, x);
    }
    {
        std::function<double(double)> dd = (double(*)(double))&sin;
        dd(1.0 * M_PI / 2);
        dd = [](double x) { return sin(x); };
        double y = M_PI * 2;
        dd = [&y](double x) { return sin(x + y); };
        dd(1.0 * M_PI / 2);
    }
    {
        double z0 = 10, z1 = 10, z2 = 10;
        std::function<double(double, double)> add2 =
            [z0, z1, z2](double x, double y)
            {
                return x + y + z0 + z1 + z2;
            };
        if (!add2)
        {
            add2 = [](double x, double y) { return x + y; };
        }
        double seven = add2(3, 4);
        EXPECT_EQ(37.0, seven);
    }
    {
        Heavy heavy;
        heavy.values = std::vector<double>{ 1, 2, 3, 4, 5 };
        heavy.values2 = std::vector<double>{ 1, 2, 3, 4, 5 };
        std::function<double(double, double, double, double)> inner_prod =
            [heavy](double x, double y, double z, double w) mutable
            {
                return heavy.method(x, y, z, w);
            };
        if (!inner_prod)
        {
            inner_prod = [](double x, double y, double z, double w) { return x+y+z+w; };
        }
        double ip_value = inner_prod(1, 0, 0, 0);
        EXPECT_EQ(30.0, ip_value);
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
        double z0 = 10, z1 = 10, z2 = 10;
        nstd::function<double(double, double)> add2 =
            [z0, z1, z2](double x, double y)
            {
                return x + y + z0 + z1 + z2;
            };
        if (!add2)
        {
            add2 = [](double x, double y) { return x + y; };
        }
        double seven = add2(3, 4);
        EXPECT_EQ(37.0, seven);
    }
    {
        Heavy heavy;
        heavy.values = std::vector<double>{ 1, 2, 3, 4, 5 };
        heavy.values2 = std::vector<double>{ 1, 2, 3, 4, 5 };
        nstd::function<double(double, double, double, double)> inner_prod =
            [heavy](double x, double y, double z, double w) mutable
            {
                return heavy.method(x, y, z, w);
            };
        if (!inner_prod)
        {
            inner_prod = [](double x, double y, double z, double w) { return x + y + z + w; };
        }
        double ip_value = inner_prod(1, 0, 0, 0);
        EXPECT_EQ(30.0, ip_value);
    }
    {
        nstd::function<int(
            int, int, int, int,
            int, int, int, int,
            int, int, int, int,
            int, int, int, int,
            int, int, int, int,
            int, int, int, int,
            int, int, int, int,
            int, int, int, int)> fn =
            [](
                int, int, int, int,
                int, int, int, int,
                int, int, int, int,
                int, int, int, int,
                int, int, int, int,
                int, int, int, int,
                int, int, int, int,
                int, int, int, int)
        {
            return 3;
        };
        if (!fn)
        {
            fn = nullptr;
        }
        int x = fn(1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8);
        EXPECT_EQ(3, x);
    }
}
