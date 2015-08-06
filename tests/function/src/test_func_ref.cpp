#define _USE_MATH_DEFINES
#include <math.h>
#include "gtest/gtest.h"
#include <functional>
#include <nstd/func_ref.hpp>

double add10(double x)
{
    return x + 10;
}

TEST(Function, func_base_ctor)
{
    {
        nstd::func_base<double, double> dd;
        EXPECT_TRUE(!dd);
    }
    {
        nstd::func_base<double, double> dd = nullptr;
        EXPECT_TRUE(!dd);
        dd = &sin;
        EXPECT_TRUE((bool)dd);
        dd = nullptr;
        EXPECT_TRUE(!dd);
    }
    {
        nstd::func_base<double, double> dd = &sin;
        EXPECT_TRUE(!!dd);
        double one = dd(1.0 * M_PI / 2);
        EXPECT_EQ(1.0, one);
        nstd::func_base<double, double> de = dd;
        EXPECT_EQ(1.0, de(1.0 * M_PI / 2));
    }
    {
        auto dsin = [](double x) { return sin(x); };
        nstd::func_base<double, double> dd = { dsin };
        EXPECT_TRUE(!!dd);
        double one = dd(1.0 * M_PI / 2);
        EXPECT_EQ(1.0, one);
    }
    {
        auto dsin = [](double x) { return sin(x); };
        nstd::func_base<double, double> dd = dsin;
        EXPECT_TRUE(!!dd);
        double one = dd(1.0 * M_PI / 2);
        EXPECT_EQ(1.0, one);
    }
    {
        double y = M_PI * 2;
        auto dsin = [&y](double x) { return sin(x + y); };
        nstd::func_base<double, double> dd = dsin;
        EXPECT_TRUE((bool)dd);
        double one = dd(1.0 * M_PI / 2);
        EXPECT_EQ(1.0, one);
    }
    {
        nstd::func_base<double, double> dd = [](double x) { return sin(x); };
        EXPECT_TRUE(!!dd);
        double one = dd(1.0 * M_PI / 2);
        EXPECT_EQ(1.0, one);
    }
}

TEST(Function, func_base_assign)
{
    nstd::func_base<double, double> dd = &sin;
    EXPECT_EQ(1.0, dd(1.0 * M_PI / 2));
    dd = &cos;
    EXPECT_EQ(1.0, dd(0.0));
    dd = nstd::func_base<double, double>();
    EXPECT_TRUE(!dd);
    dd = nstd::func_base<double, double>(&fabs);
    EXPECT_TRUE((bool)dd);
    EXPECT_EQ(1.0, dd(-1.0));
    dd = nstd::func_base<double, double>([](double) {return 1.0;});
    EXPECT_EQ(1.0, dd(0.0));
    dd = &fabs;
    EXPECT_TRUE((bool)dd);
    EXPECT_EQ(1.0, dd(-1.0));
}


class NonCopyable
{
    NonCopyable(const NonCopyable& rhs);
    NonCopyable(NonCopyable&& rhs);
    NonCopyable& operator=(const NonCopyable& rhs);
    NonCopyable& operator=(NonCopyable&& rhs);
public:
    int x;
    NonCopyable() : x(10) {}
};

void ProcessNonCopyable(NonCopyable& nc)
{
    char s[10];
    sprintf(s, "%d", nc.x);
}

void CallPNC(nstd::func_ref<void(NonCopyable&)> fref)
{
    NonCopyable nc;
    fref(nc);
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
        EXPECT_EQ(0.0, dd(0.0));
    }
    {
        nstd::func_ref<double(double)> dd = [](double x) { return sin(x); };
        EXPECT_TRUE(!!dd);
        EXPECT_EQ(0.0, dd(0.0));
    }
    {
        nstd::func_ref<double(double)> dd = &add10;
        double thirteen = dd(3.0);
        EXPECT_EQ(13.0, thirteen);
    }
    {
        CallPNC(&ProcessNonCopyable);
    }
}
