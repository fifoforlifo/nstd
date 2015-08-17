#include "gtest/gtest.h"
#include <functional>
#include <nstd/any/holder.hpp>
#include <nstd/any/value_any.hpp>
#include "test_type_erasure_common.h"

TEST(TypeErasure, value_any)
{
    typedef nstd::value_any<IRobot> AnyRobot;

    {
        AnyRobot pRobotA = DummyRobot();
        EXPECT_EQ(3, pRobotA->mana);
        AnyRobot pRobotB = CrazyRobot();
        EXPECT_EQ(1000, pRobotB->mana);

        pRobotB = pRobotA;
        EXPECT_EQ(3, pRobotB->mana);
        EXPECT_TRUE(&pRobotA->mana != &pRobotB->mana);
    }
    {
        AnyRobot pRobotA = DummyRobot();
        EXPECT_EQ(3, pRobotA->mana);
        AnyRobot pRobotB = pRobotA;
        EXPECT_EQ(3, pRobotA->mana);
        EXPECT_EQ(3, pRobotB->mana);
        AnyRobot pRobotC = std::move(pRobotB);
        EXPECT_EQ(3, pRobotA->mana);
        EXPECT_TRUE(!pRobotB);
        EXPECT_EQ(3, pRobotC->mana);
    }
}
