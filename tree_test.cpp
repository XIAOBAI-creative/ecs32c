#include <gtest/gtest.h>
#include <string>
#include <algorithm>
#include <random>
#include <ranges>
#include "tree.hpp"

TEST(TreeTest, BasicTests)
{
    auto rng = std::default_random_engine{};
    std::vector<std::string> testStrings = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"};
    std::vector<int> testValues = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    BinaryTree<std::string, int> b;

    for (auto i : std::views::iota(0, 100))
    {
        // These tests are NOT complete.  In particular it does
        // not include tests for the iterator and similar functionality
        // since those will crash in the current form, but you add tests
        // for all the functionality here and you'll probably be OK if you also
        // check for code coverage.

        // But this should give you a good idea of strategy to use...
        (void)i;
        BinaryTree<std::string, int> c;

        // This reorders the testStrings vector which you can use
        // for various testing purposes.  Since we do this a hundred
        // times and the testStrings array is different each time,
        // we expect this to provide good coverage if you test
        // all the functionality.
        std::shuffle(testStrings.begin(), testStrings.end(), rng);

        auto j = 0;
        for (auto s : testStrings)
        {
            b[s] = testValues[j];
            c[s] = testValues[j];
            EXPECT_EQ(testValues[j], b[s]);
        }

        std::string res = "";
        for (const auto &[key, value] : c)
        {
            res += key;
        }
        EXPECT_EQ(res, "ABCDEFGHIJ");
    }
    b["fubar"] = 43;
    b["baz"] = 62;
    EXPECT_EQ(b["fubar"], 43);
    EXPECT_EQ(b["baz"], 62);
}
TEST(TreeTest, insertretri)
{
    BinaryTree<std::string, int> tree;//empty tree first, then insert keys, and check keys
    tree["a"] = 1;
    tree["aa"] = 2;
    tree["aaa"] = 3;
    EXPECT_EQ(tree["a"], 1);
    EXPECT_EQ(tree["aa"], 2);
    EXPECT_EQ(tree["aaa"], 3);
}
TEST(TreeTest, Contain)
{//check tree contain key
    BinaryTree<std::string, int> tree;
    tree["b"] = 4;
    tree["bb"] = 5;
    EXPECT_TRUE(tree.contains("b"));
    EXPECT_TRUE(tree.contains("bb"));
    EXPECT_FALSE(tree.contains("bbb"));
}
TEST(TreeTest, deletee)
{
    BinaryTree<std::string, int> tree;//check delet and check other
    tree["q"] = 1;
    tree["qq"] = 2;
    tree.erase("qq");
    EXPECT_FALSE(tree.contains("qq"));
    EXPECT_TRUE(tree.contains("q"));
}
TEST(TreeTest, iteratortraval)
{//use itreator traveral tree, check key in order or not
    BinaryTree<int, std::string> tree;
    tree[3] = "th";
    tree[2] = "t";
    tree[1] = "o";
    std::string result;
    for (const auto &[key, value] : tree)
    {
        result += value + " ";
    }
    EXPECT_EQ(result, "o t th ");
}
TEST(TreeTest, empty)//check empty contain some key which should not be there, and try to delete that key
{
    BinaryTree<int, int> tree;
    EXPECT_FALSE(tree.contains(0));
    tree.erase(0);
}
TEST(TreeTest, update)//check update key
{
    BinaryTree<std::string, int> tree;
    tree["midterm"] = 16;
    tree["midterm"] = 46;
    EXPECT_EQ(tree["midterm"], 46);
}
