#include <math.h>
#include <assert.h>
#include <nstd/function.hpp>

int main()
{
    nstd::func_base<double, double> d2d;
    d2d = &fabs;

    double x = d2d(-1);
    assert(x == 1);

    return 0;
}
