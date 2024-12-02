#include <gtest/gtest.h>

TEST(Sample, Add)
{
    EXPECT_EQ(1 + 1, 3);  // FIXME
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
