#include "selectors/selectors.hpp"

#include <gtest/gtest.h>
#include <string>

// Checks for a single selector of a specific type and returns it.
template<typename T>
const T& single_selector(const std::string s) {
    Selectors res = parse_selectors(s.begin(), s.end());

    auto selectors = res.get();
    EXPECT_EQ(selectors.size(), 1) << "Not 1 root selectors found";

    auto first_selector = selectors[0].get();
    EXPECT_EQ(first_selector.size(), 1) << "Not 1 first selector found";

    return first_selector[0].as<T>();
}

TEST(SelectorParser, KeySelector) {
    std::string s = "\"key\"";
    auto selector = single_selector<KeySelector>(s);
    EXPECT_EQ(selector.get(), "key");
}

TEST(SelectorParser, IndexSelector) {
    std::string s = "[1]";
    auto selector = single_selector<IndexSelector>(s);
    EXPECT_EQ(selector.index, 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
