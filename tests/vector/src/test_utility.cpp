#include "gtest/gtest.h"
#include <vector>
#include <nstd/utility.hpp>
#include "test_vector_common.h"

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
