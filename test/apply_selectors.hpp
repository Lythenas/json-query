#include <catch/catch.hpp>
#include <string>

#include "selectors/selectors.hpp"
#include "json/json.hpp"

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

