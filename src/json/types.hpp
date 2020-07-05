#ifndef JSON_QUERY_JSON_TYPE_HPP
#define JSON_QUERY_JSON_TYPE_HPP

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <iostream>
#include <utility>
#include <vector>

namespace json {

    class JsonNode;
    std::ostream& operator<<(std::ostream& o, const JsonNode& self);

    class JsonString {
        std::string str;

       public:
        JsonString() = default;
        JsonString(std::string str) : str(std::move(str)) {}

        bool operator==(const JsonString&) const = default;

        friend std::ostream& operator<<(std::ostream& o, const JsonString& self);
    };

    class JsonNumber {
        std::string number;

       public:
        JsonNumber(std::string s) : number(std::move(s)) {}

        bool operator==(const JsonNumber&) const = default;

        friend std::ostream& operator<<(std::ostream& o, const JsonNumber& self) {
            // o << self.number;
            // if (self.fraction) {
            //     o << '.' << self.fraction.value();
            // }
            // if (self.exponent) {
            //     o << 'e' << self.exponent.value();
            // }
            // return o;
            return o << self.number;
        }
    };

    class JsonObject {
       public:
        std::vector<std::pair<std::string, JsonNode>> members;

        JsonObject() = default;
        JsonObject(const std::vector<std::pair<std::string, JsonNode>>& members)
            : members(members) {}
        // needed by parser
        JsonObject(
            const boost::optional<std::vector<std::pair<std::string, JsonNode>>>&
                members) {
            if (members) {
                this->members = boost::get(members);
            }
        }

        bool operator==(const JsonObject&) const;
    };

    class JsonArray {
       public:
        std::vector<JsonNode> items;

        JsonArray() = default;
        JsonArray(std::vector<JsonNode> items) : items(std::move(items)) {}
        // needed by parser
        JsonArray(boost::optional<std::vector<JsonNode>> items) {
            if (items) {
                this->items = boost::get(items);
            }
        }

        bool operator==(const JsonArray&) const = default;
    };

    enum JsonLiteralValue { JSON_TRUE, JSON_FALSE, JSON_NULL };

    class JsonLiteral {
        JsonLiteralValue value;

       public:
        JsonLiteral(JsonLiteralValue value) : value(value) {}

        bool operator==(const JsonLiteral&) const = default;

        friend std::ostream& operator<<(std::ostream& o, const JsonLiteral& self) {
            switch (self.value) {
                case JSON_TRUE:
                    return o << "true";
                case JSON_FALSE:
                    return o << "false";
                case JSON_NULL:
                    return o << "null";
            }
        }
    };

    class JsonNode {
        using InnerVariant = boost::variant<JsonString, JsonNumber, JsonObject,
                                            JsonArray, JsonLiteral>;

       public:
        JsonNode() = default;
        JsonNode(JsonString inner) : inner(inner) {}
        JsonNode(JsonNumber inner) : inner(inner) {}
        JsonNode(JsonObject inner) : inner(inner) {}
        JsonNode(JsonArray inner) : inner(inner) {}
        JsonNode(JsonLiteral inner) : inner(inner) {}

        bool operator==(const JsonNode&) const = default;

        template <typename T>
        const T& as() const {
            return boost::get<T>(inner);
        }

        friend std::ostream& operator<<(std::ostream& o, const JsonNode& self) {
            boost::apply_visitor(ostream_visitor(o), self.inner);
            return o;
        }

        InnerVariant inner;

       private:
        struct ostream_visitor : public boost::static_visitor<> {
            std::ostream& o;
            ostream_visitor(std::ostream& o) : o(o) {}
            template <typename T>
            void operator()(T& operand) const {
                o << operand;
            }
        };
    };

    // has to be here instead of as friend functions because we need complete type
    // of JsonNode.
    std::ostream& operator<<(std::ostream& o, const JsonObject& self);
    std::ostream& operator<<(std::ostream& o, const JsonArray& self);

    class Json {
       public:
        Json() = default;
        Json(JsonNode node) : node(std::move(node)) {}

        const JsonNode& get() const { return node; }

        friend std::ostream& operator<<(std::ostream& o, const Json& self) {
            return o << self.node;
        }

       private:
        JsonNode node;
    };

}  // namespace json

#endif
