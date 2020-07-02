#include "selectors/selectors.hpp"
#include "selectors/types.hpp"

#include <gtest/gtest.h>
#include <string>

// Checks for a single selector of a specific type and returns it.
template<typename T>
const T single_selector(const std::string s) {
    Selectors res = parse_selectors(s.begin(), s.end());

    auto selectors = res.get();
    EXPECT_EQ(selectors.size(), 1) << "Not 1 root selectors found";

    auto first_selector = selectors[0].get();
    EXPECT_EQ(first_selector.size(), 1) << "Not 1 first selector found";

    // need to copy because it would go out of scope otherwise
    return {first_selector[0].as<T>()};
}

// Checks for a single selector chain.
const std::vector<SelectorNode> single_chain(const std::string s) {
    Selectors res = parse_selectors(s.begin(), s.end());

    auto selectors = res.get();
    EXPECT_EQ(selectors.size(), 1) << "Not 1 root selectors found";

    auto chain = selectors[0].get();
    EXPECT_GT(chain.size(), 0) << "Chain is empty";
    return {chain};
}

const std::vector<RootSelector> multiple(const std::string s) {
    Selectors res = parse_selectors(s.begin(), s.end());
    auto selectors = res.get();
    EXPECT_GT(selectors.size(), 0) << "Selectors empty";

    return {selectors};
}

TEST(SelectorParser, AnyRootSelector) {
    {
        std::string s = R"#(.)#";
        auto selector = single_selector<AnyRootSelector>(s);
        // no content
    }
}

TEST(SelectorParser, KeySelector) {
    {
        std::string s = R"#("")#";
        auto selector = single_selector<KeySelector>(s);
        EXPECT_EQ(selector.get(), "") << "Empty key";
    }
    {
        std::string s = R"#("key")#";
        auto selector = single_selector<KeySelector>(s);
        EXPECT_EQ(selector.get(), "key") << "Simple key";
    }
    {
        std::string s = R"#("bigger key")#";
        auto selector = single_selector<KeySelector>(s);
        EXPECT_EQ(selector.get(), "bigger key") << "Bigger key";
    }
    {
        std::string s = R"#("'a',.c02")#";
        auto selector = single_selector<KeySelector>(s);
        EXPECT_EQ(selector.get(), "'a',.c02") << "Key with special characters";
    }
}

TEST(SelectorParser, IndexSelector) {
    {
        std::string s = R"#([1])#";
        auto selector = single_selector<IndexSelector>(s);
        EXPECT_EQ(selector.index, 1);
    }
    {
        std::string s = R"#([500])#";
        auto selector = single_selector<IndexSelector>(s);
        EXPECT_EQ(selector.index, 500);
    }
}

TEST(SelectorParser, RangeSelector) {
    {
        std::string s = R"#([1:2])#";
        auto selector = single_selector<RangeSelector>(s);
        EXPECT_EQ(selector.get_start(), 1);
        EXPECT_EQ(selector.get_end(), 2);
    }
    {
        std::string s = R"#([:2])#";
        auto selector = single_selector<RangeSelector>(s);
        EXPECT_EQ(selector.get_start(), boost::none);
        EXPECT_EQ(selector.get_end(), 2);
    }
    {
        std::string s = R"#([1:])#";
        auto selector = single_selector<RangeSelector>(s);
        EXPECT_EQ(selector.get_start(), 1);
        EXPECT_EQ(selector.get_end(), boost::none);
    }
    {
        std::string s = R"#([:])#";
        auto selector = single_selector<RangeSelector>(s);
        EXPECT_EQ(selector.get_start(), boost::none);
        EXPECT_EQ(selector.get_end(), boost::none);
    }
    {
        std::string s = R"#([])#";
        auto selector = single_selector<RangeSelector>(s);
        EXPECT_EQ(selector.get_start(), boost::none);
        EXPECT_EQ(selector.get_end(), boost::none);
    }
}

TEST(SelectorParser, PropertySelector) {
    // {
    //     std::string s = R"#({})#";
    //     auto selector = single_selector<PropertySelector>(s);
    //     auto keys = selector.get_keys();
    //     EXPECT_EQ(keys.size(), 0);
    // }
    {
        std::string s = R"#({"key1"})#";
        auto selector = single_selector<PropertySelector>(s);
        auto keys = selector.get_keys();
        EXPECT_EQ(keys.size(), 1);
        EXPECT_EQ(keys[0].get(), "key1");
    }
    {
        std::string s = R"#({"key1","key2"})#";
        auto selector = single_selector<PropertySelector>(s);
        auto keys = selector.get_keys();
        EXPECT_EQ(keys.size(), 2);
        EXPECT_EQ(keys[0].get(), "key1");
        EXPECT_EQ(keys[1].get(), "key2");
    }
}

TEST(SelectorParser, FilterSelector) {
    {
        std::string s = R"#(|"key")#";
        auto selector = single_selector<FilterSelector>(s);
        auto key = selector.get();
        EXPECT_EQ(key.get(), "key");
    }
}

TEST(SelectorParser, TruncateSelector) {
    {
        std::string s = R"#(!)#";
        auto selector = single_selector<TruncateSelector>(s);
        // no content
    }
}

TEST(SelectorParser, KeyChain) {
    {
        std::string s = R"#("key1"."key2"."key3")#";
        auto selectors = single_chain(s);
        EXPECT_EQ(selectors.size(), 3);
        EXPECT_EQ(selectors[0].as<KeySelector>().get(), "key1");
        EXPECT_EQ(selectors[1].as<KeySelector>().get(), "key2");
        EXPECT_EQ(selectors[2].as<KeySelector>().get(), "key3");
    }
}

TEST(SelectorParser, IndexChain) {
    {
        std::string s = R"#([0][1][2])#";
        auto selectors = single_chain(s);
        EXPECT_EQ(selectors.size(), 3);
        EXPECT_EQ(selectors[0].as<IndexSelector>().get(), 0);
        EXPECT_EQ(selectors[1].as<IndexSelector>().get(), 1);
        EXPECT_EQ(selectors[2].as<IndexSelector>().get(), 2);
    }
}

TEST(SelectorParser, MultipleKeys) {
    {
        std::string s = R"#("key1", "key2", "key3")#";
        auto selectors = multiple(s);
        EXPECT_EQ(selectors.size(), 3);

        {
            auto key1 = selectors[0].get();
            EXPECT_EQ(key1.size(), 1) << "First chain size is not one";
            EXPECT_EQ(key1[0].as<KeySelector>().get(), "key1");
        }
        {
            auto key2 = selectors[1].get();
            EXPECT_EQ(key2.size(), 1) << "First chain size is not one";
            EXPECT_EQ(key2[0].as<KeySelector>().get(), "key2");
        }
        {
            auto key2 = selectors[2].get();
            EXPECT_EQ(key2.size(), 1) << "First chain size is not one";
            EXPECT_EQ(key2[0].as<KeySelector>().get(), "key3");
        }
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
