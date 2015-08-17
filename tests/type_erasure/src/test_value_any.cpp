#include "gtest/gtest.h"
#include <functional>
#include <vector>
#include <nstd/any/holder.hpp>
#include <nstd/any/value_any.hpp>
#include "test_type_erasure_common.h"

TEST(TypeErasure, value_any_basic)
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

TEST(TypeErasure, value_any_as)
{
    typedef nstd::value_any<IAnimal> AnyAnimal;
    typedef nstd::value_any<IDog> AnyDog;
    typedef nstd::value_any<ICat> AnyCat;

    {
        AnyAnimal animal = AnyDog(Dog10()).move_as<IAnimal>();
        EXPECT_EQ(1, animal->Sleep());
        animal = AnyCat(Cat20()).move_as<IAnimal>();
        EXPECT_EQ(2, animal->Sleep());
        AnyCat cat = animal.move_as<ICat>();
        EXPECT_EQ(2, cat->Sleep());
        EXPECT_TRUE(!animal);
        //animal = cat.move_as<IAnimal>();
        //AnyDog dog = animal.move_as<IRobot>();  // compile error
    }
    {
        std::vector<AnyAnimal> animals;
        animals.emplace_back(AnyDog(Dog10()).move_as<IAnimal>());
        animals.emplace_back(AnyCat(Cat20()).move_as<IAnimal>());
        EXPECT_EQ(1, animals[0]->Sleep());
        EXPECT_EQ(2, animals[1]->Sleep());
    }
}
