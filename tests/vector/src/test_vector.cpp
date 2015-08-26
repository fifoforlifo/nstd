#include "gtest/gtest.h"
#include <vector>
#include <nstd/vector.hpp>
#include <nstd/utility.hpp>
#include "test_vector_common.h"

TEST(Vector, MoveSemantics)
{
    typedef nstd::vector<int> VecInt;
    VecInt intsA = { 0, 1, 2, 3 };
    EXPECT_EQ(4u, intsA.size());
    VecInt intsB = std::move(intsA);
    EXPECT_EQ(0u, intsA.size());
    EXPECT_EQ(4u, intsB.size());
}
