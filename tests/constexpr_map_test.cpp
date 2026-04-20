#include <gtest/gtest.h>

#include "spearmint/detail/constexpr_map.h"

using namespace spearmint::detail;

// ── Compile-time tests ─────────────────────────────────────────────────

constexpr auto int_map = make_sorted_map<int, const char*, 5>({{
    {3, "three"}, {1, "one"}, {5, "five"}, {2, "two"}, {4, "four"},
}});

static_assert(int_map.size() == 5);
static_assert(int_map.contains(1));
static_assert(int_map.contains(3));
static_assert(int_map.contains(5));
static_assert(!int_map.contains(0));
static_assert(!int_map.contains(6));

// Verify sorted order
static_assert(int_map.data[0].first == 1);
static_assert(int_map.data[1].first == 2);
static_assert(int_map.data[2].first == 3);
static_assert(int_map.data[3].first == 4);
static_assert(int_map.data[4].first == 5);

// ── Single element ─────────────────────────────────────────────────────

constexpr auto single_map = make_sorted_map<int, int, 1>({{
    {42, 100},
}});

static_assert(single_map.contains(42));
static_assert(!single_map.contains(0));
static_assert(single_map.at(42) == 100);

// ── Runtime tests ──────────────────────────────────────────────────────

TEST(ConstexprMapTest, FindExisting) {
    const auto* v = int_map.find(3);
    ASSERT_NE(v, nullptr);
    EXPECT_STREQ(*v, "three");
}

TEST(ConstexprMapTest, FindMissing) {
    EXPECT_EQ(int_map.find(0), nullptr);
    EXPECT_EQ(int_map.find(6), nullptr);
    EXPECT_EQ(int_map.find(-1), nullptr);
}

TEST(ConstexprMapTest, AtExisting) {
    EXPECT_STREQ(int_map.at(1), "one");
    EXPECT_STREQ(int_map.at(2), "two");
    EXPECT_STREQ(int_map.at(5), "five");
}

TEST(ConstexprMapTest, Contains) {
    EXPECT_TRUE(int_map.contains(1));
    EXPECT_TRUE(int_map.contains(4));
    EXPECT_FALSE(int_map.contains(99));
}

TEST(ConstexprMapTest, SortedOrder) {
    for (std::size_t i = 1; i < int_map.size(); ++i) {
        EXPECT_LT(int_map.data[i - 1].first, int_map.data[i].first);
    }
}

TEST(ConstexprMapTest, LargerMap) {
    constexpr auto big = make_sorted_map<int, int, 10>({{
        {100, 0}, {50, 1}, {75, 2}, {25, 3}, {10, 4},
        {90, 5}, {60, 6}, {30, 7}, {80, 8}, {40, 9},
    }});

    // Verify all found
    EXPECT_TRUE(big.contains(10));
    EXPECT_TRUE(big.contains(100));
    EXPECT_TRUE(big.contains(50));
    EXPECT_FALSE(big.contains(0));
    EXPECT_FALSE(big.contains(101));

    // Verify sorted
    for (std::size_t i = 1; i < big.size(); ++i) {
        EXPECT_LT(big.data[i - 1].first, big.data[i].first);
    }
}
