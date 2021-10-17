#include <gtest/gtest.h>
#include "../MiniViewsLib/Version.h"

TEST(VersionTest, GetDateShort)
{
    EXPECT_GT(::GetDateShort(), 0);
}
