#include <catch/catch.hpp>

#include <string>

#include "selectors/selectors.hpp"
#include "json/json.hpp"

#include <boost/optional/optional_io.hpp>

using namespace selectors;
using namespace json;

TEST_CASE("apply key chain selector", "[selectors]") {
    Json json =
        parse_json(R"#({ "key1": { "key3": 3, "key4": 4 }, "key2": 2 })#");
    Selectors selectors = parse_selectors(R"#("key1"."key3")#");
    Json result = selectors.apply(json);
    Json expected = parse_json(R"#(3)#");
    REQUIRE(result == expected);
}

TEST_CASE("apply any root selector", "[selectors]") {
    Json json = parse_json(R"#({ "key1": 1, "key2": 2 })#");
    Selectors selectors = parse_selectors(R"#(.)#");
    Json result = selectors.apply(json);
    REQUIRE(result == json);
}
TEST_CASE("apply key selector", "[selectors]") {
    Json json = parse_json(R"#({ "key1": 1, "key2": 2 })#");
    {
        Selectors selectors = parse_selectors(R"#("key1")#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#(1)#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#("key2")#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#(2)#");
        REQUIRE(result == expected);
    }
}
TEST_CASE("apply index selector", "[selectors]") {
    Json json = parse_json(R"#([1, 2, 3, 4, 5])#");
    {
        Selectors selectors = parse_selectors(R"#([0])#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#(1)#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#([3])#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#(4)#");
        REQUIRE(result == expected);
    }
}
TEST_CASE("apply range selector", "[selectors]") {
    Json json = parse_json(R"#([1, 2, 3, 4, 5])#");
    {
        Selectors selectors = parse_selectors(R"#([0:2])#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#([1, 2, 3])#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#([2:])#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#([3, 4, 5])#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#([:1])#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#([1, 2])#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#([:])#");
        Json result = selectors.apply(json);
        REQUIRE(result == json);
    }
    {
        Selectors selectors = parse_selectors(R"#([])#");
        Json result = selectors.apply(json);
        REQUIRE(result == json);
    }
}
TEST_CASE("apply property selector", "[selectors]") {
    Json json = parse_json(R"#({ "key1": 1, "key2": 2, "key3": 3 })#");
    {
        Selectors selectors = parse_selectors(R"#({ "key1" })#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#({ "key1": 1 })#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#({ "key1", "key2" })#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#({ "key1": 1, "key2": 2 })#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#({ "key1", "key3" })#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#({ "key1": 1, "key3": 3 })#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#({ "key2", "key3" })#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#({ "key2": 2, "key3": 3 })#");
        REQUIRE(result == expected);
    }
}
TEST_CASE("apply filter selector", "[selectors]") {
    Json json = parse_json(
        R"#([{"key1": 1}, {"key2": 2}, {"key1": 3}, {"key3": 4}, 5])#");
    {
        Selectors selectors = parse_selectors(R"#(|"key1")#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#([1, 3])#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#(|"key2")#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#([2])#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#(|"key3")#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#([4])#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#(|"something")#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#([])#");
        REQUIRE(result == expected);
    }
}
TEST_CASE("apply truncate selector", "[selectors]") {
    {
        Json json = parse_json(R"#([1, 2, 3])#");
        Selectors selectors = parse_selectors(R"#(!)#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#([])#");
        REQUIRE(result == expected);
    }
    {
        Json json = parse_json(R"#({"key1": 1, "key2": 2})#");
        Selectors selectors = parse_selectors(R"#(!)#");
        Json result = selectors.apply(json);
        Json expected = parse_json(R"#({})#");
        REQUIRE(result == expected);
    }
    {
        Json json = parse_json(R"#(2)#");
        Selectors selectors = parse_selectors(R"#(!)#");
        Json result = selectors.apply(json);
        REQUIRE(result == json);
    }
    {
        Json json = parse_json(R"#("abc")#");
        Selectors selectors = parse_selectors(R"#(!)#");
        Json result = selectors.apply(json);
        REQUIRE(result == json);
    }
}
TEST_CASE("apply flatten selector", "[selectors]") {
    {
        Json json = parse_json(R"#([[1, 2], [3, 4], [5, 6]])#");
        Json expected = parse_json(R"#([1, 2, 3, 4, 5, 6])#");
        Selectors selectors = parse_selectors(R"#(..)#");
        Json result = selectors.apply(json);
        REQUIRE(result == expected);
    }
    {
        Json json = parse_json(R"#({"key": [[1, 2], [3, 4], [5, 6]] })#");
        Json expected = parse_json(R"#([1, 2, 3, 4, 5, 6])#");
        Selectors selectors = parse_selectors(R"#("key"..)#");
        Json result = selectors.apply(json);
        REQUIRE(result == expected);
    }
    {
        Json json = parse_json(
            R"#([{"key": [1, 2]}, {"key": [3, 4]}, {"key": [5, 6]}])#");
        Json expected = parse_json(R"#([1, 2, 3, 4, 5, 6])#");
        Selectors selectors = parse_selectors(R"#(.."key")#");
        Json result = selectors.apply(json);
        REQUIRE(result == expected);
    }
    {
        Json json = parse_json(
            R"#({"key1": [{"key2": [1, 2]}, {"key2": [3, 4]}, {"key2": [5, 6]}] })#");
        Json expected = parse_json(R"#([1, 2, 3, 4, 5, 6])#");
        Selectors selectors = parse_selectors(R"#("key1".."key2")#");
        Json result = selectors.apply(json);
        REQUIRE(result == expected);
    }
}

TEST_CASE("apply multiple root selectors", "[selectors]") {
    Json json = parse_json(R"#({ "key1": 1, "key2": 2 })#");
    Selectors selectors = parse_selectors(R"#(.,.,.)#");
    Json result = selectors.apply(json);
    auto array = result.get().as<JsonArray>().get();
    REQUIRE(array.size() == 3);
    REQUIRE(array[0] == json.get());
    REQUIRE(array[1] == json.get());
    REQUIRE(array[2] == json.get());
}

// Checks for a single selector of a specific type and returns it.
template <typename T> T single_selector(const std::string& s) {
    Selectors res = parse_selectors(s.begin(), s.end());

    auto selectors = res.get();
    REQUIRE(selectors.size() == 1);

    auto first_selector = selectors[0].get();
    REQUIRE(first_selector.size() == 1);

    // need to copy because it would go out of scope otherwise
    return {first_selector[0].as<T>()};
}

// Checks for a single selector chain.
std::vector<SelectorNode> single_chain(const std::string& s) {
    Selectors res = parse_selectors(s.begin(), s.end());

    auto selectors = res.get();
    REQUIRE(selectors.size() == 1);

    auto chain = selectors[0].get();
    REQUIRE(!chain.empty());
    return {chain};
}

std::vector<RootSelector> multiple(const std::string& s) {
    Selectors res = parse_selectors(s.begin(), s.end());
    const auto& selectors = res.get();
    REQUIRE(!selectors.empty());

    return {selectors};
}

TEST_CASE("any root selector parses", "[selectors]") {
    std::string s = R"#(.)#";
    single_selector<AnyRootSelector>(s);
    // no content
}

TEST_CASE("key selector parses", "[selectors]") {
    {
        std::string s = R"#("")#";
        auto selector = single_selector<KeySelector>(s);
        REQUIRE(selector.get().empty());
    }
    {
        std::string s = R"#("key")#";
        auto selector = single_selector<KeySelector>(s);
        REQUIRE(selector.get() == "key");
    }
    {
        std::string s = R"#("bigger key")#";
        auto selector = single_selector<KeySelector>(s);
        REQUIRE(selector.get() == "bigger key");
    }
    {
        std::string s = R"#("'a',.c02")#";
        auto selector = single_selector<KeySelector>(s);
        REQUIRE(selector.get() == "'a',.c02");
    }
}

TEST_CASE("index selector parses", "[selectors]") {
    {
        std::string s = R"#([1])#";
        auto selector = single_selector<IndexSelector>(s);
        REQUIRE(selector.index == 1);
    }
    {
        std::string s = R"#([500])#";
        auto selector = single_selector<IndexSelector>(s);
        REQUIRE(selector.index == 500);
    }
}

TEST_CASE("range selector parses", "[selectors]") {
    {
        std::string s = R"#([1:2])#";
        auto selector = single_selector<RangeSelector>(s);
        REQUIRE(selector.get_start() == 1);
        REQUIRE(selector.get_end() == 2);
    }
    {
        std::string s = R"#([:2])#";
        auto selector = single_selector<RangeSelector>(s);
        REQUIRE(selector.get_start() == boost::none);
        REQUIRE(selector.get_end() == 2);
    }
    {
        std::string s = R"#([1:])#";
        auto selector = single_selector<RangeSelector>(s);
        REQUIRE(selector.get_start() == 1);
        REQUIRE(selector.get_end() == boost::none);
    }
    {
        std::string s = R"#([:])#";
        auto selector = single_selector<RangeSelector>(s);
        REQUIRE(selector.get_start() == boost::none);
        REQUIRE(selector.get_end() == boost::none);
    }
    {
        std::string s = R"#([])#";
        auto selector = single_selector<RangeSelector>(s);
        REQUIRE(selector.get_start() == boost::none);
        REQUIRE(selector.get_end() == boost::none);
    }
}

TEST_CASE("property selector parses", "[selectors]") {
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
        REQUIRE(keys.size() == 1);
        REQUIRE(keys[0] == "key1");
    }
    {
        std::string s = R"#({"key1","key2"})#";
        auto selector = single_selector<PropertySelector>(s);
        auto keys = selector.get_keys();
        REQUIRE(keys.size() == 2);
        REQUIRE(keys[0] == "key1");
        REQUIRE(keys[1] == "key2");
    }
}

TEST_CASE("filter selector parses", "[selectors]") {
    {
        std::string s = R"#(|"key")#";
        auto selector = single_selector<FilterSelector>(s);
        const auto& key = selector.get();
        REQUIRE(key.get() == "key");
    }
}

TEST_CASE("truncate selector parses", "[selectors]") {
    {
        std::string s = R"#(!)#";
        single_selector<TruncateSelector>(s);
        // no content
    }
}

TEST_CASE("flatten selector parses", "[selectors]") {
    {
        std::string s = R"#(..)#";
        single_selector<FlattenSelector>(s);
        // no content
    }
    {
        std::string s = R"#("key1".."key2")#";
        auto selectors = single_chain(s);
        REQUIRE(selectors.size() == 3);
        REQUIRE(selectors[0].as<KeySelector>().get() == "key1");
        selectors[1].as<FlattenSelector>(); // exception if wrong selector type
        REQUIRE(selectors[2].as<KeySelector>().get() == "key2");
    }
}

TEST_CASE("key selector chain parses", "[selectors]") {
    {
        std::string s = R"#("key1"."key2"."key3")#";
        auto selectors = single_chain(s);
        REQUIRE(selectors.size() == 3);
        REQUIRE(selectors[0].as<KeySelector>().get() == "key1");
        REQUIRE(selectors[1].as<KeySelector>().get() == "key2");
        REQUIRE(selectors[2].as<KeySelector>().get() == "key3");
    }
}

TEST_CASE("index selector chain parses", "[selectors]") {
    {
        std::string s = R"#([0][1][2])#";
        auto selectors = single_chain(s);
        REQUIRE(selectors.size() == 3);
        REQUIRE(selectors[0].as<IndexSelector>().get() == 0);
        REQUIRE(selectors[1].as<IndexSelector>().get() == 1);
        REQUIRE(selectors[2].as<IndexSelector>().get() == 2);
    }
}

TEST_CASE("multiple root key selectors parse", "[selectors]") {
    std::string s = R"#("key1", "key2", "key3")#";
    auto selectors = multiple(s);
    REQUIRE(selectors.size() == 3);

    {
        auto key1 = selectors[0].get();
        REQUIRE(key1.size() == 1); // First chain size is not one
        REQUIRE(key1[0].as<KeySelector>().get() == "key1");
    }
    {
        auto key2 = selectors[1].get();
        REQUIRE(key2.size() == 1); // First chain size is not one
        REQUIRE(key2[0].as<KeySelector>().get() == "key2");
    }
    {
        auto key2 = selectors[2].get();
        REQUIRE(key2.size() == 1); // First chain size is not one
        REQUIRE(key2[0].as<KeySelector>().get() == "key3");
    }
}
