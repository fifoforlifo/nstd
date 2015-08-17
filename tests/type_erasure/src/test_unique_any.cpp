#include "gtest/gtest.h"
#include <functional>
#include <nstd/any/holder.hpp>
#include <nstd/any/unique_any.hpp>
#include "test_type_erasure_common.h"

TEST(TypeErasure, unique_any)
{
    typedef nstd::unique_any<IRobot> AnyRobot;
    {
        DummyRobot dummyRobot;

        AnyRobot pRobot = dummyRobot;
        EXPECT_EQ(3, pRobot->mana);
        EXPECT_EQ(10, pRobot->GetHealth());

        pRobot = CrazyRobot();
        EXPECT_EQ(1, pRobot->GetHealth());
        EXPECT_EQ(1000, pRobot->mana);

        CrazyRobot crazyRobot;
        pRobot = crazyRobot;
        EXPECT_EQ(1, pRobot->GetHealth());
        EXPECT_EQ(1000, pRobot->mana);
    }
    {
        AnyRobot pRobot = DummyRobot();
        EXPECT_EQ(3, pRobot->mana);
        EXPECT_EQ(10, pRobot->GetHealth());
    }
    {
        AnyRobot pRobot = DummyRobot();
        EXPECT_EQ(3, pRobot->mana);
        pRobot = AnyRobot(CrazyRobot());
        EXPECT_EQ(1000, pRobot->mana);
        AnyRobot pRobot2;
        //pRobot = pRobot2;  // compile error
        pRobot = std::move(pRobot2);
        EXPECT_TRUE(!pRobot);
    }
}

TEST(TypeErasure, unique_any_as)
{
    typedef nstd::unique_any<IAnimal> AnyAnimal;
    typedef nstd::unique_any<IDog> AnyDog;
    typedef nstd::unique_any<ICat> AnyCat;

    {
        AnyAnimal animal = AnyDog(Dog10()).move_as<IAnimal>();
        EXPECT_EQ(1, animal->Sleep());
        animal = AnyCat(Cat20()).move_as<IAnimal>();
        EXPECT_EQ(2, animal->Sleep());
    }
}
