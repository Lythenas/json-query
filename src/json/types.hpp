#ifndef JSON_QUERY_JSON_TYPE_HPP
#define JSON_QUERY_JSON_TYPE_HPP

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/variant/detail/apply_visitor_binary.hpp>
#include <boost/variant/static_visitor.hpp>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

#include "../utils.hpp"

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
    // TODO is this enough to represent unicode
    // (theoretically yes because we don't manipulate the string)
    std::string str;

public:
    JsonString() = default;
    JsonString(std::string str) : str(std::move(str)) {}

    static const char* name() { return "String"; }

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

    static const char* name() { return "Number"; }

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
 *
 * NOTE: If there are duplicate keys we only keep the first one.
 */
class JsonObject {
    // store members in a map for faster lookup
    std::map<std::string, JsonNode> members;
    // store order to print it in same order it war originally parsed in
    // NOTE: this could maybe be made more efficient
    std::vector<std::string> order;

public:
    JsonObject() = default;
    // used by parser
    explicit JsonObject(
        const std::vector<std::pair<std::string, JsonNode>>& members);

    static const char* name() { return "Object"; }

    /**
     * Returns a reference to the value or throws std::out_of_range if not
     * found.
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
    // used by parser
    JsonArray(std::vector<JsonNode> items) : items(std::move(items)) {}

    static const char* name() { return "Array"; }

    const std::vector<JsonNode>& get() const { return items; }

    const JsonNode& at(std::size_t index) const { return items.at(index); }

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

    static const char* name() { return "Literal"; }

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
 * Constraint on the type of a json item.
 *
 * All json item types should be listed here.
 */
// clang-format off
template<typename J>
concept is_json_item =
    basically_same_as<J, JsonString> ||
    basically_same_as<J, JsonNumber> ||
    basically_same_as<J, JsonObject> ||
    basically_same_as<J, JsonArray> ||
    basically_same_as<J, JsonLiteral>;
// clang-format on

/**
 * Unifying wrapper for all json item kinds.
 *
 * This avoids having to allocate each constructor separately on the heap.
 */
class JsonNode {
    using InnerVariant = boost::variant<JsonString, JsonNumber, JsonObject,
                                        JsonArray, JsonLiteral>;

    InnerVariant inner;

public:
    JsonNode() = default;
    // used by the parser
    template <is_json_item J> JsonNode(J inner) : inner(inner) {}

    const char* name() const {
        return boost::apply_visitor(
            [](is_json_item auto& x) { return x.name(); }, inner);
    }

    bool operator==(const JsonNode&) const = default;

    template <is_json_item J> const J& as() const {
        return boost::get<J>(inner);
    }

    /**
     * Allow visitation lambdas.
     */
    template <typename Visitor, typename... Args>
    decltype(auto) apply_visitor(Visitor&& visitor, Args... args) const {
        return boost::apply_visitor(visitor, inner, args...);
    }

    friend std::ostream& operator<<(std::ostream& o, const JsonNode& self) {
        auto print = [&o](is_json_item auto& operand) { o << operand; };
        boost::apply_visitor(print, self.inner);
        return o;
    }
};

// class JsonString
std::ostream& operator<<(std::ostream& o, const JsonString& self) {
    return o << "\"" << self.str << "\"";
}

// class JsonObject
JsonObject::JsonObject(
    const std::vector<std::pair<std::string, JsonNode>>& members) {
    // We need to do manual iteration because the string in the pair can't be a
    // "const std::string". It is not possible to cast the template parameters
    // like that and the parser can't produce the correct type.  This is a
    // limitation of the C++ type system.
    for (auto [key, value] : members) {
        // ignore duplicate keys
        if (this->members.contains(key)) {
            continue;
        }

        const std::string key_copy{key};
        this->members[key] = value;
        this->order.push_back(key_copy);
    }
}
const JsonNode& JsonObject::find(const std::string& key) const {
    return members.at(key);
}
bool JsonObject::operator==(const JsonObject& other) const {
    return this->members == other.members;
}
std::ostream& operator<<(std::ostream& o, const JsonObject& self) {
    o << "{";

    const char* sep = "";
    for (const auto& key : self.order) {
        o << sep << "\"" << key << "\":" << self.members.at(key);
        sep = ",";
    }

    return o << "}";
}

// class JsonArray
std::ostream& operator<<(std::ostream& o, const JsonArray& self) {
    o << "[";

    const char* sep = "";
    for (const auto& i : self.items) {
        o << sep << i;
        sep = ",";
    }

    return o << "]";
}

} // namespace json

#endif
