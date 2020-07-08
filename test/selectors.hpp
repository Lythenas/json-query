#include <gtest/gtest.h>

#include <string>

#include "json/json.hpp"
#include "selectors/parser.hpp"
#include "selectors/selectors.hpp"

using namespace selectors;
using namespace json;

TEST(SelectorApplication, KeySelectorChain) {
    {
        Json json = parse_json(R"#({ "key1": { "key3": 3, "key4": 4 }, "key2": 2 })#");
        Selectors selectors = parse_selectors(R"#("key1"."key3")#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#(3)#");
        EXPECT_EQ(result, expected);
    }
}

TEST(SelectorApplication, AnyRootSelector) {
    {
        Json json = parse_json(R"#({ "key1": 1, "key2": 2 })#");
        Selectors selectors = parse_selectors(R"#(.)#");
        Json result = selectors.apply(json);
        EXPECT_EQ(result, json);
    }
}
TEST(SelectorApplication, KeySelector) {
    {
        Json json = parse_json(R"#({ "key1": 1, "key2": 2 })#");
        {
            Selectors selectors = parse_selectors(R"#("key1")#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#(1)#");
            EXPECT_EQ(result, expected);
        }
        {
            Selectors selectors = parse_selectors(R"#("key2")#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#(2)#");
            EXPECT_EQ(result, expected);
        }
    }
}
TEST(SelectorApplication, IndexSelector) {
    {
        Json json = parse_json(R"#([1, 2, 3, 4, 5])#");
        {
            Selectors selectors = parse_selectors(R"#([0])#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#(1)#");
            EXPECT_EQ(result, expected);
        }
        {
            Selectors selectors = parse_selectors(R"#([3])#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#(4)#");
            EXPECT_EQ(result, expected);
        }
    }
}
TEST(SelectorApplication, RangeSelector) {
    {
        Json json = parse_json(R"#([1, 2, 3, 4, 5])#");
        {
            Selectors selectors = parse_selectors(R"#([0:2])#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#([1, 2, 3])#");
            EXPECT_EQ(result, expected);
        }
        {
            Selectors selectors = parse_selectors(R"#([2:])#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#([3, 4, 5])#");
            EXPECT_EQ(result, expected);
        }
        {
            Selectors selectors = parse_selectors(R"#([:1])#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#([1, 2])#");
            EXPECT_EQ(result, expected);
        }
        {
            Selectors selectors = parse_selectors(R"#([:])#");
            Json result = selectors.apply(json);
            EXPECT_EQ(result, json);
        }
        {
            Selectors selectors = parse_selectors(R"#([])#");
            Json result = selectors.apply(json);
            EXPECT_EQ(result, json);
        }
    }
}
TEST(SelectorApplication, PropertySelector) {
    {
        Json json = parse_json(R"#({ "key1": 1, "key2": 2, "key3": 3 })#");
        {
            Selectors selectors = parse_selectors(R"#({ "key1" })#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#({ "key1": 1 })#");
            EXPECT_EQ(result, expected);
        }
        {
            Selectors selectors = parse_selectors(R"#({ "key1", "key2" })#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#({ "key1": 1, "key2": 2 })#");
            EXPECT_EQ(result, expected);
        }
        {
            Selectors selectors = parse_selectors(R"#({ "key1", "key3" })#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#({ "key1": 1, "key3": 3 })#");
            EXPECT_EQ(result, expected);
        }
        {
            Selectors selectors = parse_selectors(R"#({ "key2", "key3" })#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#({ "key2": 2, "key3": 3 })#");
            EXPECT_EQ(result, expected);
        }
    }
}
TEST(SelectorApplication, FilterSelector) {
    {
        Json json = parse_json(
            R"#([{"key1": 1}, {"key2": 2}, {"key1": 3}, {"key3": 4}, 5])#");
        {
            Selectors selectors = parse_selectors(R"#(|"key1")#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#([1, 3])#");
            EXPECT_EQ(result, expected);
        }
        {
            Selectors selectors = parse_selectors(R"#(|"key2")#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#([2])#");
            EXPECT_EQ(result, expected);
        }
        {
            Selectors selectors = parse_selectors(R"#(|"key3")#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#([4])#");
            EXPECT_EQ(result, expected);
        }
        {
            Selectors selectors = parse_selectors(R"#(|"something")#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#([])#");
            EXPECT_EQ(result, expected);
        }
    }
}
TEST(SelectorApplication, TruncateSelector) {
    {
        {
            Json json = parse_json(R"#([1, 2, 3])#");
            Selectors selectors = parse_selectors(R"#(!)#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#([])#");
            EXPECT_EQ(result, expected);
        }
        {
            Json json = parse_json(R"#({"key1": 1, "key2": 2})#");
            Selectors selectors = parse_selectors(R"#(!)#");
            Json result = selectors.apply(json);
            Json expected = parse_json(R"#({})#");
            EXPECT_EQ(result, expected);
        }
        {
            Json json = parse_json(R"#(2)#");
            Selectors selectors = parse_selectors(R"#(!)#");
            Json result = selectors.apply(json);
            EXPECT_EQ(result, json);
        }
        {
            Json json = parse_json(R"#("abc")#");
            Selectors selectors = parse_selectors(R"#(!)#");
            Json result = selectors.apply(json);
            EXPECT_EQ(result, json);
        }
    }
}

TEST(SelectorApplication, MultipleRootSelectors) {
    {
        Json json = parse_json(R"#({ "key1": 1, "key2": 2 })#");
        Selectors selectors = parse_selectors(R"#(.,.,.)#");
        Json result = selectors.apply(json);
        auto array = result.get().as<JsonArray>().get();
        EXPECT_EQ(array.size(), 3);
        EXPECT_EQ(array[0], json.get());
        EXPECT_EQ(array[1], json.get());
        EXPECT_EQ(array[2], json.get());
    }
}

// Checks for a single selector of a specific type and returns it.
template <typename T>
T single_selector(const std::string& s) {
    Selectors res = parse_selectors(s.begin(), s.end());

    auto selectors = res.get();
    EXPECT_EQ(selectors.size(), 1) << "Not 1 root selectors found";

    auto first_selector = selectors[0].get();
    EXPECT_EQ(first_selector.size(), 1) << "Not 1 first selector found";

    // need to copy because it would go out of scope otherwise
    return {first_selector[0].as<T>()};
}

// Checks for a single selector chain.
std::vector<SelectorNode> single_chain(const std::string& s) {
    Selectors res = parse_selectors(s.begin(), s.end());

    auto selectors = res.get();
    EXPECT_EQ(selectors.size(), 1) << "Not 1 root selectors found";

    auto chain = selectors[0].get();
    EXPECT_GT(chain.size(), 0) << "Chain is empty";
    return {chain};
}

std::vector<RootSelector> multiple(const std::string& s) {
    Selectors res = parse_selectors(s.begin(), s.end());
    const auto& selectors = res.get();
    EXPECT_GT(selectors.size(), 0) << "Selectors empty";

    return {selectors};
}

TEST(SelectorParser, AnyRootSelector) {
    {
        std::string s = R"#(.)#";
        single_selector<AnyRootSelector>(s);
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
        EXPECT_EQ(keys[0], "key1");
    }
    {
        std::string s = R"#({"key1","key2"})#";
        auto selector = single_selector<PropertySelector>(s);
        auto keys = selector.get_keys();
        EXPECT_EQ(keys.size(), 2);
        EXPECT_EQ(keys[0], "key1");
        EXPECT_EQ(keys[1], "key2");
    }
}

TEST(SelectorParser, FilterSelector) {
    {
        std::string s = R"#(|"key")#";
        auto selector = single_selector<FilterSelector>(s);
        const auto& key = selector.get();
        EXPECT_EQ(key.get(), "key");
    }
}

TEST(SelectorParser, TruncateSelector) {
    {
        std::string s = R"#(!)#";
        single_selector<TruncateSelector>(s);
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
