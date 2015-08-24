#include "gtest/gtest.h"
#include <vector>
#include <nstd/vector.hpp>
#include <nstd/utility.hpp>

struct ConOnly
{
    int& value;

    ConOnly(int& value_)
        : value(value_)
    {}
    ConOnly(const ConOnly& rhs)
        : value(rhs.value)
    {}
};
class Swappable
{
    Swappable(const Swappable& rhs);
public:
    int value;

    Swappable(int value_)
        : value(value_)
    {}

    Swappable& swap(Swappable& rhs)
    {
        int tmp = value;
        value = rhs.value;
        rhs.value = tmp;
        return *this;
    }
};

TEST(Utility, GenericAssign)
{
    int x = 3;
    ConOnly coA = x;
    int y = 4;
    ConOnly coB = y;
    EXPECT_EQ(&x, &coA.value);
    EXPECT_EQ(&y, &coB.value);
    // coA = coB; // illegal
    nstd::assign(coA, coB);
    EXPECT_EQ(&y, &coA.value);
}

TEST(Utility, Swap)
{
    {
        int x = 3;
        ConOnly coA = x;
        int y = 4;
        ConOnly coB = y;
        EXPECT_EQ(&x, &coA.value);
        EXPECT_EQ(&y, &coB.value);
        nstd::swap(coA, coB);
        EXPECT_EQ(&y, &coA.value);
        EXPECT_EQ(&x, &coB.value);
    }
    {
        Swappable swA(3);
        Swappable swB(4);
        EXPECT_EQ(3, swA.value);
        EXPECT_EQ(4, swB.value);
        nstd::swap(swA, swB);
        EXPECT_EQ(4, swA.value);
        EXPECT_EQ(3, swB.value);
    }
}
