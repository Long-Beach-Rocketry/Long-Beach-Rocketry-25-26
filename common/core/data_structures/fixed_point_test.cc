#include "fixed_point.h"
#include <gtest/gtest.h>

using namespace LBR;

class FixedPointTest : public testing ::Test
{
public:
};

TEST_F(FixedPointTest, CompareTest)
{
    fixed32_32 number(10, 5);
    EXPECT_TRUE(number.to_float() == 10.5f);
}