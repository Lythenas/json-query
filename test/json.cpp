#include <boost/none.hpp>
#include <string>
#include <gtest/gtest.h>

#include "json/json.hpp"
#include <utility>

template<typename T>
T single_node(const std::string& s) {
    return parse_json(s.begin(), s.end()).get().as<T>();
}

TEST(JsonParser, Number_Int) {
    {
        std::string s = R"#(5)#";
        auto number = single_node<JsonNumber>(s);
        EXPECT_EQ(number, JsonNumber("5"));
    }
    {
        std::string s = R"#(-105)#";
        auto number = single_node<JsonNumber>(s);
        EXPECT_EQ(number, JsonNumber("-105"));
    }
}

TEST(JsonParser, Number_Float) {
    {
        std::string s = R"#(5.25)#";
        auto number = single_node<JsonNumber>(s);
        EXPECT_EQ(number, JsonNumber("5.25"));
    }
    {
        std::string s = R"#(-0.000125)#";
        auto number = single_node<JsonNumber>(s);
        EXPECT_EQ(number, JsonNumber("-0.000125"));
    }
}

TEST(JsonParser, Number_Int_Exp) {
    {
        std::string s = R"#(5e100)#";
        auto number = single_node<JsonNumber>(s);
        EXPECT_EQ(number, JsonNumber("5e100"));
    }
}

TEST(JsonParser, Number_Float_Exp) {
    {
        std::string s = R"#(5.25e100)#";
        auto number = single_node<JsonNumber>(s);
        EXPECT_EQ(number, JsonNumber("5.25e100"));
    }
}

TEST(JsonParser, Literal) {
    {
        std::string s = R"#(true)#";
        auto lit = single_node<JsonLiteral>(s);
        EXPECT_EQ(lit, JsonLiteral(JSON_TRUE));
    }
    {
        std::string s = R"#(false)#";
        auto lit = single_node<JsonLiteral>(s);
        EXPECT_EQ(lit, JsonLiteral(JSON_FALSE));
    }
    {
        std::string s = R"#(null)#";
        auto lit = single_node<JsonLiteral>(s);
        EXPECT_EQ(lit, JsonLiteral(JSON_NULL));
    }
}

TEST(JsonParser, String) {
    {
        std::string s = R"#("Hello, World!")#";
        auto str = single_node<JsonString>(s);
        EXPECT_EQ(str, JsonString("Hello, World!"));
    }
    {
        std::string s = R"#("Hello, World! \" \\ \n \b \f \r \t \u22")#";
        auto str = single_node<JsonString>(s);
        EXPECT_EQ(str, JsonString("Hello, World! \\\" \\\\ \\n \\b \\f \\r \\t \\u22"));
    }
}

TEST(JsonParser, Array) {
    {
        std::string s = R"#([])#";
        auto arr = single_node<JsonArray>(s);
        EXPECT_EQ(arr, JsonArray());
    }
    {
        std::string s = R"#([1])#";
        auto arr = single_node<JsonArray>(s);
        EXPECT_EQ(arr, JsonArray(std::vector{JsonNode(JsonNumber("1"))}));
    }
    {
        std::string s = R"#([1, 2, 3])#";
        auto arr = single_node<JsonArray>(s);
        EXPECT_EQ(arr, JsonArray(std::vector{JsonNode(JsonNumber("1")), JsonNode(JsonNumber("2")), JsonNode(JsonNumber("3"))}));
    }
}

TEST(JsonParser, Object) {
    {
        std::string s = R"#({})#";
        auto obj = single_node<JsonObject>(s);
        EXPECT_EQ(obj, JsonObject());
    }
    {
        std::string s = R"#({ "key1": 5 })#";
        auto obj = single_node<JsonObject>(s);
        EXPECT_EQ(obj, JsonObject(
            std::vector{
                std::make_pair(std::string{"key1"}, JsonNode(JsonNumber("5")))
            }
        ));
    }
    {
        std::string s = R"#({ "key1": 5, "key2": 7 })#";
        auto obj = single_node<JsonObject>(s);
        EXPECT_EQ(obj, JsonObject(
            std::vector{
                std::make_pair(std::string{"key1"}, JsonNode(JsonNumber("5"))),
                std::make_pair(std::string{"key2"}, JsonNode(JsonNumber("7")))
            }
        ));
    }
}
