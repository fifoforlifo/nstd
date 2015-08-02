#define _USE_MATH_DEFINES
#include <math.h>
#include "gtest/gtest.h"
#include <functional>
#include <nstd/func_ref.hpp>

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

double add10(double x)
{
    return x + 10;
}

TEST(Function, aaa)
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
#endif
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
        double y = M_PI * 2;
        auto dsin = [&y](double x) { return sin(x + y); };
        nstd::func_base<double, double> dd = dsin;
        EXPECT_TRUE((bool)dd);
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
    nstd::func_base<double, double> dd = &sin;
    EXPECT_EQ(dd(1.0 * M_PI / 2), 1.0);
    dd = &cos;
    EXPECT_EQ(dd(0.0), 1.0);
    dd = nstd::func_base<double, double>();
    EXPECT_TRUE(!dd);
    dd = nstd::func_base<double, double>(&fabs);
    EXPECT_TRUE((bool)dd);
    EXPECT_EQ(dd(-1.0), 1.0);
    dd = nstd::func_base<double, double>([](double) {return 1.0;});
    EXPECT_EQ(dd(0.0), 1.0);
    dd = &fabs;
    EXPECT_TRUE((bool)dd);
    EXPECT_EQ(dd(-1.0), 1.0);
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
    {
        nstd::func_ref<double(double)> dd = &add10;
        double thirteen = dd(3.0);
        EXPECT_EQ(thirteen, 13.0);
    }
    {
        CallPNC(&ProcessNonCopyable);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}