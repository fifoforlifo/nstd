#include <math.h>
#include <assert.h>
#include <nstd/function.hpp>

int main()
{
    std::function<double(double)> ddd = (double(*)(double))&fabs;
    ddd = std::function<double(double)>([](double) {return 1.0;});
    ddd(1.0);

    nstd::func_base<double, double> d2d = &sin;
    assert(d2d);
    d2d = &cos;
    d2d = nstd::func_base<double, double>();
    assert(!d2d);
    d2d = nstd::func_base<double, double>(&fabs);
    d2d = nstd::func_base<double, double>([](double) {return 1.0;});
    d2d(1.0);
    d2d = &fabs;

    double x = d2d(-1);
    assert(x == 1);

    nstd::func_ref<double(*)(double)> dd;
    dd = &fabs;

    return 0;
}
