#if defined(_MSC_VER) || defined(__GNUC__)

#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include <functional>
#include "nstd/function.hpp"

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#if defined(_MSC_VER)
int64_t readtsc()
{
    return (int64_t)__rdtsc();
}
#elif defined(__GNUC__)
int64_t readtsc()
{
#if defined(__i386__)
    long long a;
    asm volatile("rdtsc":"=A" (a));
    return (int64_t)a;
#elif defined(__x86_64)
    unsigned int _hi, _lo;
    asm volatile("rdtsc":"=a"(_lo), "=d"(_hi));
    return ((int64_t)_hi << 32) | _lo;
#else
    // ARM etc.
    return 0;
#endif
}
#endif

template <class Func>
double TimeIt(int count, const char* pName, Func&& func)
{
    for (int ii = 0; ii < count; ++ii) {
        func();
    }

    double totDuration = 0;

    for (int ii = 0; ii < count; ++ii) {
        auto startTime = readtsc();

        func();

        auto endTime = readtsc();

        totDuration += double(endTime - startTime);
    }

    double avgDuration = totDuration / count;
    std::cout << pName << " took " << avgDuration << " cycles." << std::endl;

    return avgDuration;
}

inline double add10(double x)
{
    return x + 10;
}
double add20(double x)
{
    return x + 20;
}

bool g_false = false;
double g_sum = 0.0;

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

static void test(double(*dd)(double))
{
    if (g_false) {
        dd = &add20;
    }
    g_sum = 0;
    int NUM_VALUES = 10 * 1000 * 1000;
    double sum = 0;
    for (int i = 0; i < NUM_VALUES; ++i) {
        sum += dd(i);
    }
    g_sum = sum;
}

template <class Function>
static void test(Function dd)
{
    if (g_false) {
        dd = &add20;
    }
    g_sum = 0;
    int NUM_VALUES = 10 * 1000 * 1000;
    double sum = 0;
    for (int i = 0; i < NUM_VALUES; ++i) {
        sum += dd(i);
    }
    g_sum = sum;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

void VerifySum()
{
    int NUM_VALUES = 10 * 1000 * 1000;
    double sum = 0;
    for (int i = 0; i < NUM_VALUES; ++i) {
        sum += (i + 10);
    }
    if (sum != g_sum)
    {
        exit(1);
    }
}

int main(int argc, char** argv)
{
    (void)argv;
    if (argc > 3)
        g_false = true;

    //double y = 0;
    nstd::function<double(double)> nstd_fn = [=](double x) { return add10(x + 0); };
    std::function<double(double)> std_fn = [=](double x) { return add10(x + 0); };
    double(*fnptr)(double) = &add10;
    TimeIt(10, "nstd::function", [&]() { test(nstd_fn); });
    VerifySum();
    TimeIt(10, " std::function", [&]() { test(std_fn); });
    VerifySum();
    TimeIt(10, "function ptr  ", [&]() { test(fnptr); });
    VerifySum();

    double nstd_val = TimeIt(10, "nstd::function", [&]() { test(nstd_fn); });
    VerifySum();
    double std_val  = TimeIt(10, " std::function", [&]() { test(std_fn); });
    VerifySum();
    double fnp_val  = TimeIt(10, "function ptr  ", [&]() { test(fnptr); });
    VerifySum();

    std::cout << " std / fnp = " <<  std_val / fnp_val << std::endl;
    std::cout << "nstd / fnp = " << nstd_val / fnp_val << std::endl;
    std::cout << "nstd / std = " << nstd_val / std_val << std::endl;

    return 0;
}

#else

int main()
{
    return 0;
}

#endif
