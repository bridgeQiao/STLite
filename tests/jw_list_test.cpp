#include "gtest/gtest.h"
#include "stlite/jw_algorithm.hpp"
#include "stlite/jw_list.hpp"

TEST(jwList, initialize) {
    int a[] = { 1,2,3,4,5,6,7,8,9 };
    jw::list<int> ilist(a, a + 9);
    EXPECT_EQ(ilist.size(), 9);
    ilist.pop_back();
    EXPECT_EQ(ilist.size(), 8);
}
