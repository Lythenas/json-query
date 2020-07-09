#ifndef JSON_QUERY_JSON_TYPE_HPP
#define JSON_QUERY_JSON_TYPE_HPP

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/variant/detail/apply_visitor_binary.hpp>
#include <boost/variant/static_visitor.hpp>
#include <iostream>
#include <utility>
#include <vector>

namespace json {

    class JsonNode;
    std::ostream& operator<<(std::ostream& o, const JsonNode& self);

    /**
     * A string in json.
     *
     * Contains the content of the string. I.e. what's inbetween the `"`s.
     *
     * E.g.
     *
     * ```json
     * "something"
     * ```
     */
    class JsonString {
        std::string str;

       public:
        JsonString() = default;
        JsonString(std::string str) : str(std::move(str)) {}

        static const char* name() {
            return "String";
        }

        bool operator==(const JsonString&) const = default;

        friend std::ostream& operator<<(std::ostream& o, const JsonString& self);
    };

    /**
     * A number in json.
     *
     * Contains the string that was used to represent the number in json. This
     * is done so we can output the number exactly the same as in the input and
     * there is no conversion between exponention and normal notation and there
     * is also no loss of precision in floats and no limit to the size of ints.
     *
     * E.g.
     *
     * ```json
     * 22
     * 2.5
     * -10e22
     * ```
     */
    class JsonNumber {
        std::string number;

       public:
        JsonNumber(std::string s) : number(std::move(s)) {}

        static const char* name() {
            return "Number";
        }

        bool operator==(const JsonNumber&) const = default;

        friend std::ostream& operator<<(std::ostream& o, const JsonNumber& self) {
            return o << self.number;
        }
    };

    /**
     * An object in json.
     *
     * Contains a vector of key-value pairs. It is possible to have duplicate
     * keys and the order of the keys is preserved. The keys are stored as
     * strings (like in JsonString).
     *
     * Objects can be empty.
     *
     * E.g.
     *
     * ```json
     * {
     *   "key1": 22,
     *   "key2": 42
     * }
     * ```
     */
    class JsonObject {
        std::vector<std::pair<std::string, JsonNode>> members;

       public:
        JsonObject() = default;
        JsonObject(const std::vector<std::pair<std::string, JsonNode>>& members)
            : members(members) {}
        // needed by parser
        explicit JsonObject(
            const boost::optional<std::vector<std::pair<std::string, JsonNode>>>&
                members) {
            if (members) {
                this->members = boost::get(members);
            }
        }

        static const char* name() {
            return "Object";
        }

        /**
         * Returns a reference to the value or throws std::out_of_range if not found.
         */
        const JsonNode& find(const std::string& key) const;

        bool operator==(const JsonObject&) const;

        friend std::ostream& operator<<(std::ostream&, const JsonObject&);
    };

    /**
     * An array in json.
     *
     * Contains a vector of items. Can be empty.
     *
     * E.g.
     *
     * ```json
     * [1, 2, 3]
     * ```
     */
    class JsonArray {
        std::vector<JsonNode> items;

       public:
        JsonArray() = default;
        JsonArray(std::vector<JsonNode> items) : items(std::move(items)) {}
        // needed by parser
        explicit JsonArray(boost::optional<std::vector<JsonNode>> items) {
            if (items) {
                this->items = boost::get(items);
            }
        }

        static const char* name() {
            return "Array";
        }

        const std::vector<JsonNode>& get() const {
            return items;
        }

        const JsonNode& at(std::size_t index) const {
            return items.at(index);
        }

        bool operator==(const JsonArray&) const = default;

        friend std::ostream& operator<<(std::ostream&, const JsonArray&);
    };

    enum JsonLiteralValue {
        /**
         * Json literal `true`.
         */
        JSON_TRUE,
        /**
         * Json literal `false`.
         */
        JSON_FALSE,
        /**
         * Json literal `null`.
         */
        JSON_NULL
    };

    /**
     * A literal in json.
     *
     * See JsonLiteralValue.
     */
    class JsonLiteral {
        JsonLiteralValue value;

       public:
        JsonLiteral(JsonLiteralValue value) : value(value) {}

        static const char* name() {
            return "Literal";
        }

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

    /**
     * Unifying wrapper for all json item kinds.
     *
     * This avoids having to allocate each constructor separately on the heap.
     */
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

        const char* name() const {
            return boost::apply_visitor([](auto& x) { return x.name(); }, inner);
        }

        bool operator==(const JsonNode&) const = default;

        template <typename T>
        const T& as() const {
            return boost::get<T>(inner);
        }

        /**
         * Allow visitation lambdas.
         */
        template <typename Visitor>
        decltype(auto) apply_visitor(Visitor&& visitor) const {
            return boost::apply_visitor(visitor, inner);
        }
        template <typename Visitor, typename ...Args>
        decltype(auto) apply_visitor(Visitor&& visitor, Args ...args) const {
            return boost::apply_visitor(visitor, inner, args...);
        }

        friend std::ostream& operator<<(std::ostream& o, const JsonNode& self) {
            auto print = [&o](auto& operand) {
                o << operand;
            };
            boost::apply_visitor(print, self.inner);
            return o;
        }

       private:
        InnerVariant inner;
    };

    /**
     * Wrapper for an entire json *document*.
     *
     * Simply wraps a JsonNode.
     *
     * TODO not sure if we need this
     */
    class Json {
       public:
        Json() = default;
        Json(JsonNode node) : node(std::move(node)) {}

        const JsonNode& get() const { return node; }
        JsonNode& get() { return node; }

        bool operator==(const Json&) const = default;

        friend std::ostream& operator<<(std::ostream& o, const Json& self) {
            return o << self.node;
        }

       private:
        JsonNode node;
    };

}  // namespace json

#endif
