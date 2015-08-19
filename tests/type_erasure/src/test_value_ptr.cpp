#include "gtest/gtest.h"
#include <nstd/ptr/value_ptr_nd.hpp>
#include <nstd/ptr/value_ptr_nd_sbo.hpp>

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


TEST(TypeErasure, value_ptr_nd)
{
    typedef nstd::value_ptr_nd<INum> INumValuePtr;
    typedef nstd::value_ptr_nd<IBigNum> BigNumValuePtr;
    typedef nstd::value_ptr_nd<Num> NumValuePtr;

    {
        INumValuePtr pNumA = new Num{ 3 };
        EXPECT_EQ(3, pNumA->Value());
        INumValuePtr pNumB = pNumA;
        EXPECT_EQ(3, pNumB->Value());
        EXPECT_TRUE(&*pNumB != &*pNumA);
        BigNumValuePtr pNumC = pNumB.copy_as<IBigNum>();
        EXPECT_EQ(3, pNumC->Value());
        EXPECT_TRUE(&*pNumC != &*pNumB);
        IBigNum* pNumC_old = &*pNumC;
        NumValuePtr pNumD = pNumC.move_as<Num>();
        EXPECT_TRUE(!pNumC);
        EXPECT_EQ(pNumC_old, &*pNumD);
    }
}

TEST(TypeErasure, value_ptr_nd_sbo)
{
    typedef nstd::value_ptr_nd_sbo<INum, 0x80> INumValuePtr;
    typedef nstd::value_ptr_nd_sbo<IBigNum, 0x80> BigNumValuePtr;
    typedef nstd::value_ptr_nd_sbo<Num, 0x80> NumValuePtr;

    {
        INumValuePtr pNumA = new Num{ 3 };
        EXPECT_EQ(3, pNumA->Value());
        INumValuePtr pNumB = pNumA;
        EXPECT_EQ(3, pNumB->Value());
        EXPECT_TRUE(&*pNumB != &*pNumA);
        BigNumValuePtr pNumC = pNumB.copy_as<IBigNum>();
        EXPECT_EQ(3, pNumC->Value());
        EXPECT_TRUE(&*pNumC != &*pNumB);
        NumValuePtr pNumD = pNumC.move_as<Num>();
        EXPECT_TRUE(!pNumC);
        pNumD->m_value = 5;
        EXPECT_EQ(5, pNumD->Value());
    }
}
