#include <catch2/catch.hpp>

#include <boost/none.hpp>
#include <string>
#include <utility>

#include "json/json.hpp"
#include "json/types.hpp"

using namespace json;

template <typename T>
T single_node(const std::string& s) {
    return parse_json(s.begin(), s.end()).get().as<T>();
}

TEST_CASE("integers are parserd", "[json]") {
    {
        std::string s = R"#(5)#";
        auto number = single_node<JsonNumber>(s);
        REQUIRE(number == JsonNumber("5"));
    }
    {
        std::string s = R"#(-105)#";
        auto number = single_node<JsonNumber>(s);
        REQUIRE(number == JsonNumber("-105"));
    }
}

TEST_CASE("floats are parsed", "[json]") {
    {
        std::string s = R"#(5.25)#";
        auto number = single_node<JsonNumber>(s);
        REQUIRE(number == JsonNumber("5.25"));
    }
    {
        std::string s = R"#(-0.000125)#";
        auto number = single_node<JsonNumber>(s);
        REQUIRE(number == JsonNumber("-0.000125"));
    }
}

TEST_CASE("ints with exponents are parsed", "[json]") {
    {
        std::string s = R"#(5e100)#";
        auto number = single_node<JsonNumber>(s);
        REQUIRE(number == JsonNumber("5e100"));
    }
}

TEST_CASE("floats with exponents are parsed", "[json]") {
    {
        std::string s = R"#(5.25e100)#";
        auto number = single_node<JsonNumber>(s);
        REQUIRE(number == JsonNumber("5.25e100"));
    }
}

TEST_CASE("literals are parsed", "[json]") {
    {
        std::string s = R"#(true)#";
        auto lit = single_node<JsonLiteral>(s);
        REQUIRE(lit == JsonLiteral(JSON_TRUE));
    }
    {
        std::string s = R"#(false)#";
        auto lit = single_node<JsonLiteral>(s);
        REQUIRE(lit == JsonLiteral(JSON_FALSE));
    }
    {
        std::string s = R"#(null)#";
        auto lit = single_node<JsonLiteral>(s);
        REQUIRE(lit == JsonLiteral(JSON_NULL));
    }
}

TEST_CASE("strings are parsed", "[json]") {
    {
        std::string s = R"#("Hello, World!")#";
        auto str = single_node<JsonString>(s);
        REQUIRE(str == JsonString("Hello, World!"));
    }
    {
        std::string s = R"#("Hello, World! \" \\ \n \b \f \r \t \u22")#";
        auto str = single_node<JsonString>(s);
        REQUIRE(
            str ==
            JsonString("Hello, World! \\\" \\\\ \\n \\b \\f \\r \\t \\u22"));
    }
}

TEST_CASE("arrays of ints are parsed", "[json]") {
    {
        std::string s = R"#([])#";
        auto arr = single_node<JsonArray>(s);
        REQUIRE(arr == JsonArray());
    }
    {
        std::string s = R"#([1])#";
        auto arr = single_node<JsonArray>(s);
        REQUIRE(arr == JsonArray(std::vector{JsonNode(JsonNumber("1"))}));
    }
    {
        std::string s = R"#([1, 2, 3])#";
        auto arr = single_node<JsonArray>(s);
        REQUIRE(arr == JsonArray(std::vector{JsonNode(JsonNumber("1")),
                                             JsonNode(JsonNumber("2")),
                                             JsonNode(JsonNumber("3"))}));
    }
}

TEST_CASE("objects with ints are parsed", "[json]") {
    {
        std::string s = R"#({})#";
        auto obj = single_node<JsonObject>(s);
        REQUIRE(obj == JsonObject());
    }
    {
        std::string s = R"#({ "key1": 5 })#";
        auto obj = single_node<JsonObject>(s);
        REQUIRE(obj == JsonObject(std::vector{std::make_pair(
                           std::string{"key1"}, JsonNode(JsonNumber("5")))}));
    }
    {
        std::string s = R"#({ "key1": 5, "key2": 7 })#";
        auto obj = single_node<JsonObject>(s);
        REQUIRE(
            obj ==
            JsonObject(std::vector{
                std::make_pair(std::string{"key1"}, JsonNode(JsonNumber("5"))),
                std::make_pair(std::string{"key2"},
                               JsonNode(JsonNumber("7")))}));
    }
}

TEST_CASE("nested objects and arrays are parsed", "[json]") {
    {
        std::string s =
            R"#({ "array": [ { "id": 1, "value": "x" }, { "id": 2, "value": "y" } ] })#";
        auto obj = single_node<JsonObject>(s);
        REQUIRE(
            obj ==
            JsonObject(std::vector{std::make_pair(
                std::string{"array"},
                JsonNode(JsonArray(std::vector{
                    JsonNode(JsonObject(std::vector{
                        std::make_pair(std::string{"id"},
                                       JsonNode(JsonNumber("1"))),
                        std::make_pair(std::string{"value"},
                                       JsonNode(JsonString("x")))})),
                    JsonNode(JsonObject(std::vector{
                        std::make_pair(std::string{"id"},
                                       JsonNode(JsonNumber("2"))),
                        std::make_pair(std::string{"value"},
                                       JsonNode(JsonString("y")))}))})))}));
    }
    {
        std::string s = R"#({
    "array": [
        { "id": 1, "value": "x" },
        { "id": 2, "value": "y" }
    ]
})#";
        auto obj = single_node<JsonObject>(s);
        REQUIRE(
            obj ==
            JsonObject(std::vector{std::make_pair(
                std::string{"array"},
                JsonNode(JsonArray(std::vector{
                    JsonNode(JsonObject(std::vector{
                        std::make_pair(std::string{"id"},
                                       JsonNode(JsonNumber("1"))),
                        std::make_pair(std::string{"value"},
                                       JsonNode(JsonString("x")))})),
                    JsonNode(JsonObject(std::vector{
                        std::make_pair(std::string{"id"},
                                       JsonNode(JsonNumber("2"))),
                        std::make_pair(std::string{"value"},
                                       JsonNode(JsonString("y")))}))})))}));
    }
}
