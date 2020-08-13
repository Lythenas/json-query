#include <catch/catch.hpp>
#include <string>

#include "selectors/selectors.hpp"
#include "json/json.hpp"

using namespace selectors;
using namespace json;

TEST_CASE("apply key chain selector", "[selectors]") {
    JsonNode json =
        parse_json(R"#({ "key1": { "key3": 3, "key4": 4 }, "key2": 2 })#");
    Selectors selectors = parse_selectors(R"#("key1"."key3")#");
    JsonNode result = selectors.apply(json);
    JsonNode expected = parse_json(R"#(3)#");
    REQUIRE(result == expected);
}

TEST_CASE("apply any root selector", "[selectors]") {
    JsonNode json = parse_json(R"#({ "key1": 1, "key2": 2 })#");
    Selectors selectors = parse_selectors(R"#(.)#");
    JsonNode result = selectors.apply(json);
    REQUIRE(result == json);
}
TEST_CASE("apply key selector", "[selectors]") {
    JsonNode json = parse_json(R"#({ "key1": 1, "key2": 2 })#");
    {
        Selectors selectors = parse_selectors(R"#("key1")#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#(1)#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#("key2")#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#(2)#");
        REQUIRE(result == expected);
    }
}
TEST_CASE("apply index selector", "[selectors]") {
    JsonNode json = parse_json(R"#([1, 2, 3, 4, 5])#");
    {
        Selectors selectors = parse_selectors(R"#([0])#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#(1)#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#([3])#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#(4)#");
        REQUIRE(result == expected);
    }
}
TEST_CASE("apply range selector", "[selectors]") {
    JsonNode json = parse_json(R"#([1, 2, 3, 4, 5])#");
    {
        Selectors selectors = parse_selectors(R"#([0:2])#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#([1, 2, 3])#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#([2:])#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#([3, 4, 5])#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#([:1])#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#([1, 2])#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#([:])#");
        JsonNode result = selectors.apply(json);
        REQUIRE(result == json);
    }
    {
        Selectors selectors = parse_selectors(R"#([])#");
        JsonNode result = selectors.apply(json);
        REQUIRE(result == json);
    }
}
TEST_CASE("apply property selector", "[selectors]") {
    JsonNode json = parse_json(R"#({ "key1": 1, "key2": 2, "key3": 3 })#");
    {
        Selectors selectors = parse_selectors(R"#({ "key1" })#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#({ "key1": 1 })#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#({ "key1", "key2" })#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#({ "key1": 1, "key2": 2 })#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#({ "key1", "key3" })#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#({ "key1": 1, "key3": 3 })#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#({ "key2", "key3" })#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#({ "key2": 2, "key3": 3 })#");
        REQUIRE(result == expected);
    }
}
TEST_CASE("apply filter selector", "[selectors]") {
    JsonNode json = parse_json(
        R"#([{"key1": 1}, {"key2": 2}, {"key1": 3}, {"key3": 4}, 5])#");
    {
        Selectors selectors = parse_selectors(R"#(|"key1")#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#([1, 3])#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#(|"key2")#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#([2])#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#(|"key3")#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#([4])#");
        REQUIRE(result == expected);
    }
    {
        Selectors selectors = parse_selectors(R"#(|"something")#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#([])#");
        REQUIRE(result == expected);
    }
}
TEST_CASE("apply truncate selector", "[selectors]") {
    {
        JsonNode json = parse_json(R"#([1, 2, 3])#");
        Selectors selectors = parse_selectors(R"#(!)#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#([])#");
        REQUIRE(result == expected);
    }
    {
        JsonNode json = parse_json(R"#({"key1": 1, "key2": 2})#");
        Selectors selectors = parse_selectors(R"#(!)#");
        JsonNode result = selectors.apply(json);
        JsonNode expected = parse_json(R"#({})#");
        REQUIRE(result == expected);
    }
    {
        JsonNode json = parse_json(R"#(2)#");
        Selectors selectors = parse_selectors(R"#(!)#");
        JsonNode result = selectors.apply(json);
        REQUIRE(result == json);
    }
    {
        JsonNode json = parse_json(R"#("abc")#");
        Selectors selectors = parse_selectors(R"#(!)#");
        JsonNode result = selectors.apply(json);
        REQUIRE(result == json);
    }
}
TEST_CASE("apply flatten selector", "[selectors]") {
    {
        JsonNode json = parse_json(R"#([[1, 2], [3, 4], [5, 6]])#");
        JsonNode expected = parse_json(R"#([1, 2, 3, 4, 5, 6])#");
        Selectors selectors = parse_selectors(R"#(..)#");
        JsonNode result = selectors.apply(json);
        REQUIRE(result == expected);
    }
    {
        JsonNode json = parse_json(R"#({"key": [[1, 2], [3, 4], [5, 6]] })#");
        JsonNode expected = parse_json(R"#([1, 2, 3, 4, 5, 6])#");
        Selectors selectors = parse_selectors(R"#("key"..)#");
        JsonNode result = selectors.apply(json);
        REQUIRE(result == expected);
    }
    {
        JsonNode json = parse_json(
            R"#([{"key": [1, 2]}, {"key": [3, 4]}, {"key": [5, 6]}])#");
        JsonNode expected = parse_json(R"#([1, 2, 3, 4, 5, 6])#");
        Selectors selectors = parse_selectors(R"#(.."key")#");
        JsonNode result = selectors.apply(json);
        REQUIRE(result == expected);
    }
    {
        JsonNode json = parse_json(
            R"#({"key1": [{"key2": [1, 2]}, {"key2": [3, 4]}, {"key2": [5, 6]}] })#");
        JsonNode expected = parse_json(R"#([1, 2, 3, 4, 5, 6])#");
        Selectors selectors = parse_selectors(R"#("key1".."key2")#");
        JsonNode result = selectors.apply(json);
        REQUIRE(result == expected);
    }
}

TEST_CASE("apply multiple root selectors", "[selectors]") {
    JsonNode json = parse_json(R"#({ "key1": 1, "key2": 2 })#");
    Selectors selectors = parse_selectors(R"#(.,.,.)#");
    JsonNode result = selectors.apply(json);
    auto array = result.as<JsonArray>().get();
    REQUIRE(array.size() == 3);
    REQUIRE(array[0] == json);
    REQUIRE(array[1] == json);
    REQUIRE(array[2] == json);
}

