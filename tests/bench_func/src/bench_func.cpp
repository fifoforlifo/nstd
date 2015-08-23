#if defined(_MSC_VER) || defined(__GNUC__)

#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include <functional>
#include <time.h>
#include "nstd/function.hpp"

template <class Func>
double TimeIt(int count, const char* pName, Func&& func)
{
    double totDuration = 0;

    for (int ii = 0; ii < count; ++ii) {
        clock_t c_start = clock();

        func();

        clock_t c_end = clock();

        totDuration += double(c_end - c_start);
    }

    double avgDuration = totDuration / count;
    std::cout << pName << " took " << avgDuration / CLOCKS_PER_SEC << " seconds." << std::endl;

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

const int NUM_VALUES = 100 * 1000 * 1000;

static void test(double(*dd)(double))
{
    if (g_false) {
        dd = &add20;
    }
    g_sum = 0;
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
    double sum = 0;
    for (int i = 0; i < NUM_VALUES; ++i) {
        sum += dd(i);
    }
    g_sum = sum;
}

static void test_nstd()
{
    auto lambda = [](double x) { return add10(x); };
    nstd::function<double(double)> nstd_fn = lambda;
    test(nstd_fn);
}
static void test_std()
{
    auto lambda = [](double x) { return add10(x); };
    std::function<double(double)> nstd_fn = lambda;
    test(nstd_fn);
}
static void test_fnptr()
{
    test(&add10);
}


#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

void VerifySum()
{
    double sum = 0;
    for (int i = 0; i < NUM_VALUES; ++i) {
        sum += (i + 10);
    }
    if (sum != g_sum)
    {
        fprintf(stderr, "FAILED: expected_sum=%f observed_sum=%f\n", sum, g_sum);
        exit(1);
    }
}

int main(int argc, char** argv)
{
    (void)argv;
    if (argc > 3)
        g_false = true;

    int repeats = 1;

    TimeIt(repeats, "nstd::function", &test_nstd);
    VerifySum();
    TimeIt(repeats, " std::function", &test_std);
    VerifySum();
    TimeIt(repeats, "function ptr  ", &test_fnptr);
    VerifySum();

    double nstd_val = TimeIt(repeats, "nstd::function", &test_nstd);
    VerifySum();
    double std_val  = TimeIt(repeats, " std::function", &test_std);
    VerifySum();
    double fnp_val  = TimeIt(repeats, "function ptr  ", &test_fnptr);
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
