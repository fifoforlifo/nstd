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
struct IFloatNum
{
    virtual ~IFloatNum() {}
    virtual float ValueFloat() const = 0;
};
struct Num : public IBigNum, IFloatNum
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
    virtual float ValueFloat() const
    {
        return (float)m_value;
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
    typedef nstd::value_ptr_nd<IFloatNum> IFloatNumValuePtr;

    {
        INumValuePtr pNumA = INumValuePtr().acquire(new Num{ 3 });
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
        BigNumValuePtr pNumE = pNumD;
        EXPECT_TRUE(!!pNumD && !!pNumE);
        pNumE = std::move(pNumD);
        EXPECT_TRUE(!pNumD);
        pNumD = Num(3);
        IFloatNumValuePtr pNumF = pNumD.copy_as<IFloatNum>();
        EXPECT_EQ(3.0f, pNumF->ValueFloat());
        pNumF = pNumD.move_as<IFloatNum>();
        EXPECT_TRUE(!pNumD);
        EXPECT_EQ(3.0f, pNumF->ValueFloat());
        pNumD = pNumF.copy_as<Num>();
        EXPECT_TRUE(!!pNumD);
        Num num(8);
        pNumA = num;
        EXPECT_EQ(8, pNumA->Value());
    }
}

TEST(TypeErasure, value_ptr_nd_sbo)
{
    typedef nstd::value_ptr_nd_sbo<INum, 0x80> INumValuePtr;
    typedef nstd::value_ptr_nd_sbo<IBigNum, 0x40> BigNumValuePtr;
    typedef nstd::value_ptr_nd_sbo<Num, 0x20> NumValuePtr;
    typedef nstd::value_ptr_nd_sbo<IFloatNum, 0x30> IFloatNumValuePtr;

    {
        INumValuePtr pNumA = INumValuePtr().acquire(new Num{ 3 });
        EXPECT_EQ(3, pNumA->Value());
        INumValuePtr pNumB = pNumA;
        EXPECT_EQ(3, pNumB->Value());
        EXPECT_TRUE(&*pNumB != &*pNumA);
        BigNumValuePtr pNumC = pNumB.copy_as<IBigNum>();
        EXPECT_EQ(3, pNumC->Value());
        EXPECT_TRUE(&*pNumC != &*pNumB);
        NumValuePtr pNumD = pNumC.move_as<Num>();
        EXPECT_TRUE(!pNumC);
        EXPECT_TRUE(!!pNumD);
        pNumD = pNumA;
        EXPECT_TRUE(pNumD->Value() == pNumA->Value());
        BigNumValuePtr pNumE = pNumD;
        EXPECT_TRUE(!!pNumD && !!pNumE);
        pNumE = std::move(pNumD);
        EXPECT_TRUE(!pNumD);
        pNumD = Num(3);
        IFloatNumValuePtr pNumF = pNumD.copy_as<IFloatNum>();
        EXPECT_EQ(3.0f, pNumF->ValueFloat());
        pNumF = pNumD.move_as<IFloatNum>();
        EXPECT_TRUE(!pNumD);
        EXPECT_EQ(3.0f, pNumF->ValueFloat());
        pNumD = pNumF.copy_as<Num>();
        EXPECT_TRUE(!!pNumD);
        Num num(8);
        pNumA = num;
        EXPECT_EQ(8, pNumA->Value());
    }
}
