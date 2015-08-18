#include "gtest/gtest.h"
#include <nstd/ptr/value_ptr.hpp>

struct INum
{
    virtual ~INum() {}
    virtual int Value() const = 0;
};
struct IBigNum : public INum
{
    virtual int ValueHi() const = 0;
};
struct Num : public IBigNum
{
    int m_value;
    virtual int Value() const
    {
        return m_value;
    }
    virtual int ValueHi() const
    {
        return 0;
    }
    Num(int value)
        : m_value(value)
    {}
};


TEST(TypeErasure, value_ptr_basic)
{
    typedef nstd::value_ptr<INum> NumValuePtr;
    typedef nstd::value_ptr<IBigNum> BigNumValuePtr;

    {
        NumValuePtr pNumA = new Num{ 3 };
        EXPECT_EQ(3, pNumA->Value());
        NumValuePtr pNumB = pNumA;
        EXPECT_EQ(3, pNumB->Value());
        EXPECT_TRUE(&*pNumB != &*pNumA);
        BigNumValuePtr pNumC = pNumB.copy_as<IBigNum>();
        EXPECT_EQ(3, pNumC->Value());
        EXPECT_TRUE(&*pNumC != &*pNumB);
        IBigNum* pNumC_old = &*pNumC;
        BigNumValuePtr pNumD = pNumC.move_as<IBigNum>();
        EXPECT_TRUE(!pNumC);
        EXPECT_EQ(pNumC_old, &*pNumD);
    }
}
