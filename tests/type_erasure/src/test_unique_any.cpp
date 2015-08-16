#include "gtest/gtest.h"
#include <functional>
#include <nstd/unique_any.hpp>

struct IRobot
{
    virtual void Move(int dx, int dy) = 0;
    virtual int GetHealth() const = 0;
    int& mana;

    IRobot(int& mana_) : mana(mana_) {}
};

template <class Object>
class Holder
{
protected:
    typedef typename std::decay<Object>::type Obj;
    Obj m_obj;

public:
    Holder(Object&& obj)
        : m_obj(static_cast<Object&&>(obj))
    {
    }
};

template <class Object>
class RobotAdapter : public Holder<Object>, public IRobot
{
public:
    virtual void Move(int dx, int dy)
    {
        this->m_obj.Move(dx, dy);
    }
    virtual int GetHealth() const
    {
        return this->m_obj.GetHealth();
    }

    RobotAdapter(Object&& obj)
        : Holder<Object>(static_cast<Object&&>(obj))
        , IRobot(this->m_obj.mana)
    {
    }
};

template <class Object>
RobotAdapter<Object> select_adapter(IRobot*, Object&& obj);

struct DummyRobot
{
    void Move(int dx, int dy)
    {
        (void)dx;
        (void)dy;
    }
    int GetHealth() const
    {
        return 10;
    }
    int mana;

    DummyRobot()
        : mana(3)
    {}
};

struct CrazyRobot
{
    void Move(int dx, int dy)
    {
        (void)dx;
        (void)dy;
    }
    int GetHealth() const
    {
        return 1;
    }
    int mana;

    CrazyRobot()
        : mana(1000)
    {}
};

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
        //pRobot = pRobot2;
        pRobot = std::move(pRobot2);
        EXPECT_TRUE(!pRobot);
    }
}
