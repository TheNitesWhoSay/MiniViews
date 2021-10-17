#include <gtest/gtest.h>
#include "../MiniViewsLib/Zerocmp.h"

TEST(ZerocmpTest, isAllZero)
{
    EXPECT_FALSE(isAllZero({}));

    EXPECT_TRUE(isAllZero({0}));
    EXPECT_TRUE(isAllZero({0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_TRUE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));

    EXPECT_FALSE(isAllZero({1}));
    EXPECT_FALSE(isAllZero({0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));

    EXPECT_FALSE(isAllZero({1, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}));

    EXPECT_FALSE(isAllZero({1}));
    EXPECT_FALSE(isAllZero({1, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));

    EXPECT_FALSE(isAllZero({0, 1}));
    EXPECT_FALSE(isAllZero({0, 1, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));

    EXPECT_FALSE(isAllZero({0, 1, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 1, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 1, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 1, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 1, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 1, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 1, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    EXPECT_FALSE(isAllZero({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}));
}
