#define _USE_MATH_DEFINES
#include <math.h>
#include "gtest/gtest.h"
#include <nstd/function.hpp>

TEST(Function, aaa)
{
    {
        std::function<double(double)> dd = (double(*)(double))&fabs;
        dd = std::function<double(double)>([](double) {return 1.0;});
        dd(1.0);
    }
    {
        //std::function<double(double)> dd = [](double x) { return sin(x); };
    }
}

TEST(Function, func_base_ctor)
{
    {
        nstd::func_base<double, double> dd;
        EXPECT_FALSE(!!dd);
    }
    {
        nstd::func_base<double, double> dd = &sin;
        EXPECT_TRUE(!!dd);
        double sin1 = dd(1.0 * M_PI / 2);
        EXPECT_EQ(sin1, 1.0);
        nstd::func_base<double, double> de = dd;
        EXPECT_EQ(de(1.0 * M_PI / 2), 1.0);
    }
    {
        auto dsin = [](double x) { return sin(x); };
        nstd::func_base<double, double> dd = { dsin };
        EXPECT_TRUE(!!dd);
        double sin1 = dd(1.0 * M_PI / 2);
        EXPECT_EQ(sin1, 1.0);
    }
    {
        auto dsin = [](double x) { return sin(x); };
        nstd::func_base<double, double> dd = dsin;
        EXPECT_TRUE(!!dd);
        double sin1 = dd(1.0 * M_PI / 2);
        EXPECT_EQ(sin1, 1.0);
    }
    {
        double y = 0.0;
        auto dsin = [&y](double x) { return sin(x + y); };
        nstd::func_base<double, double> dd = dsin;
        EXPECT_TRUE(!!dd);
        double sin1 = dd(1.0 * M_PI / 2);
        EXPECT_EQ(sin1, 1.0);
    }
    {
        nstd::func_base<double, double> dd = [](double x) { return sin(x); };
        EXPECT_TRUE(!!dd);
        double sin1 = dd(1.0 * M_PI / 2);
        EXPECT_EQ(sin1, 1.0);
    }
}

TEST(Function, func_base_assign)
{
    nstd::func_base<double, double> d2d = &sin;
    d2d = &cos;
    d2d = nstd::func_base<double, double>();
    d2d = nstd::func_base<double, double>(&fabs);
    d2d = nstd::func_base<double, double>([](double) {return 1.0;});
    d2d(1.0);
    d2d = &fabs;
}

TEST(Function, func_ref_ctor)
{
    {
        nstd::func_ref<double(double)> dd;
        EXPECT_FALSE(!!dd);
    }
    {
        nstd::func_ref<double(double)> dd = &sin;
        EXPECT_TRUE(!!dd);
        EXPECT_EQ(dd(0.0), 0.0);
    }
    {
        nstd::func_ref<double(double)> dd = [](double x) { return sin(x); };
        EXPECT_TRUE(!!dd);
        EXPECT_EQ(dd(0.0), 0.0);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}