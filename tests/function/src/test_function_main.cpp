#include "gtest/gtest.h"

#if _MSC_VER
#define CDECL __cdecl
#else
#define CDECL
#endif

int CDECL main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
