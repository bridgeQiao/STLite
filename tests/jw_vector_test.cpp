#include "gtest/gtest.h"
#include "stlite/jw_vector.hpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

TEST(jwVect, initialize) {
    jw::vector<int> vi;
    EXPECT_EQ(vi.size(), 0);
    EXPECT_EQ(vi.capacity(), 0);

    int a[] = { 1,2,3,4,5,6,7,8,9 };

    vi.push_back(3);
    vi.push_back(4);
    vi.push_back(5);
    jw::vector<int> vi2(a, a + 9);
    EXPECT_EQ(vi2.size(), 9);
    EXPECT_EQ(vi2.capacity(), 9);

    vi2.insert(vi2.begin(), 7, 3);
    vi2.pop_back();
    EXPECT_EQ(vi2.size(), 15);
    EXPECT_EQ(vi2.capacity(), 18);
}
