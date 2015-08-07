#if defined(_MSC_VER)

#include <iostream>
#include <stdint.h>
#include <intrin.h>
#include <vector>
#include <functional>
#include "nstd/function.hpp"

template <class Func>
double TimeIt(int count, const char* pName, Func&& func)
{
    for (int ii = 0; ii < count; ++ii) {
        func();
    }

    double totDuration = 0;

    for (int ii = 0; ii < count; ++ii) {
        auto startTime = __rdtsc();
        _ReadWriteBarrier();

        func();

        _ReadWriteBarrier();
        auto endTime = __rdtsc();

        totDuration += (endTime - startTime);
    }

    double avgDuration = totDuration / count;
    std::cout << pName << " took " << avgDuration << " cycles." << std::endl;

    return avgDuration;
}

double add10(double x)
{
    return x + 10;
}
double add20(double x)
{
    return x + 20;
}

double g_sum = 0.0;

template <class Function>
struct Test
{
    static void invoke()
    {
        Function dd = [](double x) { return add10(x); };
        if (!dd) {
            dd = &add20;
        }
        int NUM_VALUES = 100000;
        double sum = 0;
        for (int i = 0; i < NUM_VALUES; ++i) {
            sum += dd(i);
        }
        g_sum = sum;
    }
};

int main()
{
    double fnp_val  = TimeIt(10, "function ptr  ", &Test<double(*)(double)>::invoke);
    double std_val  = TimeIt(10, " std::function", &Test<std::function<double(double)>>::invoke);
    double nstd_val = TimeIt(10, "nstd::function", &Test<nstd::function<double(double)>>::invoke);

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
