#ifndef JSON_QUERY_SELCTOR_TYPE_HPP
#define JSON_QUERY_SELCTOR_TYPE_HPP

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

#include "../json/json.hpp"
#include "../utils.hpp"

namespace selectors {
using namespace json;
namespace ranges = std::ranges;

class ApplySelectorError : public std::exception {
    std::string message;

public:
    ApplySelectorError(const std::string& message) : message(message) {}

    const char* what() const noexcept override { return message.c_str(); }
};

class SelectorNode;

// Used to detect wrong parsing because the default constructor of the
// first variant is sometimes used.
class InvalidSelector {
public:
    static const char* name() { return "Invalid"; }
    friend std::ostream& operator<<(std::ostream& o,
                                    const InvalidSelector& /*unused*/) {
        return o << "InvalidSelector";
    }
};

/**
 * Selects everything.
 *
 * Can only be the first element of a RootSelector but can be followed by other
 * selectors.
 *
 * Can be applied to all json items.
 */
class AnyRootSelector {
public:
    static const char* name() { return "Any"; }

    friend std::ostream& operator<<(std::ostream& o,
                                    const AnyRootSelector& /*unused*/) {
        return o << "AnyRootSelector";
    }
};

/**
 * Selects a single key/property from an object.
 *
 * Identified by a string surrounded with `"`.
 *
 * Can only be applied to objects.
 *
 * E.g.
 *
 * ```json
 * {
 *   "key1": 1,
 *   "key2": 2,
 *   "key3": 3
 * }
 * ```
 *
 * with the selector `"key2"` turns into
 *
 * ```json
 * 2
 * ```
 */
class KeySelector {
    std::string key;

public:
    KeySelector() = default;
    KeySelector(std::string key) : key(std::move(key)) {}
    KeySelector(const KeySelector&) = default;
    KeySelector& operator=(const KeySelector&) = default;

    static const char* name() { return "Key"; }

    const std::string& get() const { return key; }

    friend std::ostream& operator<<(std::ostream& o, const KeySelector& self) {
        return o << "KeySelector(" << self.key << ")";
    }
};

/**
 * Selects an element from an array.
 *
 * Identified by `[` and `]` with a single number in between.
 *
 * Can only be applied to arrays.
 *
 * E.g.
 *
 * ```json
 * [ 1, 2, 3 ]
 * ```
 *
 * with the selector `[1]` turns into
 *
 * ```json
 * 2
 * ```
 */
class IndexSelector {
    int index;

public:
    IndexSelector() = default;
    IndexSelector(int index) : index(index) {}
    IndexSelector(const IndexSelector&) = default;
    IndexSelector& operator=(const IndexSelector&) = default;

    static const char* name() { return "Index"; }

    int get() const { return index; }

    friend std::ostream& operator<<(std::ostream& o,
                                    const IndexSelector& self) {
        return o << "IndexSelector(" << self.index << ")";
    }
};

/**
 * Selects a range from an array.
 *
 * Identified by `[` and `]` with a range in between. The range can have a start
 * and an end separated by `:`. If there is nothing between the brackets it is
 * the same as `[:]`.
 *
 * Can only be applied to arrays.
 *
 * E.g. `[1:2]`, `[4:]`, `[:3]`, `[:]`, `[]`
 *
 * E.g.
 *
 * ```json
 * [ 1, 2, 3, 4, 5 ]
 * ```
 *
 * with the range `[1:3]` turns into
 *
 * ```json
 * [ 2, 3, 4 ]
 * ```
 */
class RangeSelector {
    // uses boost::optional instead of std::optional because that is easier to
    // use with the parser
    boost::optional<int> start;
    boost::optional<int> end;

public:
    RangeSelector() = default;
    RangeSelector(boost::optional<int> start, boost::optional<int> end)
        : start(start), end(end) {}
    RangeSelector(const RangeSelector&) = default;
    RangeSelector& operator=(const RangeSelector&) = default;

    static const char* name() { return "Range"; }

    const boost::optional<int>& get_start() const { return start; }
    const boost::optional<int>& get_end() const { return end; }

    friend std::ostream& operator<<(std::ostream& o,
                                    const RangeSelector& self) {
        return o << "RangeSelector("
                 << (self.start.has_value() ? std::to_string(self.start.value())
                                            : "")
                 << ","
                 << (self.end.has_value() ? std::to_string(self.end.value())
                                          : "")
                 << ")";
    }
};

/**
 * Selects (multiple) properties/keys from an object.
 *
 * Identified by `{` and `}` with a list of keys in between.
 *
 * Can only be applied to objects.
 *
 * E.g.
 *
 * ```json
 * {
 *   "key1": 1,
 *   "key2": 2,
 *   "key3": 3
 * }
 * ```
 *
 * with the selector `{"key1", "key3"}` turns into
 *
 * ```json
 * {
 *   "key1": 1,
 *   "key3": 3
 * }
 * ```
 */
class PropertySelector {
    std::vector<std::string> keys;

public:
    PropertySelector() = default;
    PropertySelector(std::vector<std::string> keys) : keys(std::move(keys)) {}
    PropertySelector(const PropertySelector&) = default;
    PropertySelector& operator=(const PropertySelector&) = default;

    static const char* name() { return "Property"; }

    const std::vector<std::string>& get_keys() const { return keys; }

    friend std::ostream& operator<<(std::ostream& o,
                                    const PropertySelector& self) {
        o << "PropertySelector(";
        for (const auto& x : self.keys) {
            o << x << ",";
        }
        return o << ")";
    }
};

/**
 * Selector to filter arrays.
 *
 * Identified by `|` followed by a key.
 *
 * Only works on arrays. When applied it filters the array for objects
 * containing the given key and returns an array containing the associated
 * items of the keys.
 *
 * Can only be applied to arrays.
 *
 * E.g.
 *
 * ```json
 * [
 *   { "key": 1 },
 *   { "key": 2 },
 *   { "key": 3 }
 * ]
 * ```
 *
 * with the filter `|"key"` turns into
 *
 * ```json
 * [ 1, 2, 3 ]
 * ```
 */
class FilterSelector {
    // Having a nested KeySelector makes it easier to parse and use in
    // apply_selector.
    KeySelector filter;

public:
    FilterSelector() = default;
    FilterSelector(const KeySelector& filter) : filter(filter) {}
    FilterSelector(const FilterSelector&) = default;
    FilterSelector& operator=(const FilterSelector&) = default;

    static const char* name() { return "Filter"; }

    const KeySelector& get() const { return filter; }

    friend std::ostream& operator<<(std::ostream& o,
                                    const FilterSelector& self) {
        return o << "FilterSelector(" << self.filter << ")";
    }
};

/**
 * Selector to truncate json.
 *
 * Identified by `!`.
 *
 * When applied truncates the json at the current item. Outputting the item if
 * it is a number or a string and outputting an empty object or array for
 * objects and arrays.
 *
 * Can be applied to all json items.
 */
class TruncateSelector {
public:
    static const char* name() { return "Truncate"; }

    friend std::ostream& operator<<(std::ostream& o,
                                    const TruncateSelector& /*unused*/) {
        return o << "TruncateSelector";
    }
};

/**
 * Selector to flatten json arrays.
 *
 * Identified by the prefix `..`.
 *
 * When applied will flatten one level of arrays.
 *
 * E.g.
 *
 * ```json
 * [ [1, 2], [3, 4], [5, 6] ]
 * ```
 *
 * turns into
 *
 * ```json
 * [1, 2, 3, 4, 5, 6]
 * ```
 */
class FlattenSelector {
public:
    static const char* name() { return "Flatten"; }

    friend std::ostream& operator<<(std::ostream& o,
                                    const FlattenSelector& /*unused*/) {
        return o << "FlattenSelector";
    }
};

/**
 * Constraint on the type of a selector.
 *
 * All selector types should be listed here.
 */
// clang-format off
template <typename S>
concept is_selector =
    basically_same_as<S, InvalidSelector> ||
    basically_same_as<S, AnyRootSelector> ||
    basically_same_as<S, KeySelector> ||
    basically_same_as<S, IndexSelector> ||
    basically_same_as<S, RangeSelector> ||
    basically_same_as<S, PropertySelector> ||
    basically_same_as<S, FilterSelector> ||
    basically_same_as<S, TruncateSelector> ||
    basically_same_as<S, FlattenSelector>;
// clang-format on

/**
 * Constraint for the type of iterator used for selector chains.
 *
 * Requirements:
 *
 * - copyable: need to save a copy in case of multiple sub-json items
 *   (e.g. in ranges).
 * - incrementable
 * - dereferecing yields `const SelectorNode&`
 */
// clang-format off
template <typename I>
concept sel_iter =
    std::copyable<I> &&
    std::incrementable<I> &&
    requires(I i) {
        { *i } ->std::same_as<const SelectorNode&>;
    };
// clang-format on

/**
 * Unifying wrapper for all selector kinds.
 *
 * This avoids having to allocate each constructor separately on the heap.
 */
class SelectorNode {
    using InnerVariant =
        boost::variant<InvalidSelector, AnyRootSelector, KeySelector,
                       IndexSelector, RangeSelector, PropertySelector,
                       FilterSelector, TruncateSelector, FlattenSelector>;

public:
    explicit SelectorNode() = default;

    template <is_selector S> explicit SelectorNode(S inner) : inner(inner) {}

    const char* name() const {
        return boost::apply_visitor(
            [](const is_selector auto& x) { return x.name(); }, inner);
    }

    template <is_selector S> const S& as() const {
        return boost::get<S>(inner);
    }

    template <sel_iter Iterator>
    JsonNode apply(const JsonNode& json, Iterator next, Iterator end) const {
        return json.apply_visitor(
            [next, end](const is_json_item auto& item,
                        const is_selector auto& s) {
                return s.apply(item, next, end);
            },
            inner);
    }

    friend std::ostream& operator<<(std::ostream& o, const SelectorNode& self) {
        boost::apply_visitor([&o](is_selector auto val) { o << val; },
                             self.inner);
        return o;
    }

    // private:
    InnerVariant inner;
};

template <typename T>
void extend_vec_with(std::vector<T>& vec, const std::vector<T>& extension) {
    vec.insert(vec.end(), extension.begin(), extension.end());
}

// these template functions make it a little easier to find and extend what
// selector handles what json item
//
// also: ideally I would put them into a separate header but it's to much
// hassle to resolve the "define before use" dependencies

template <sel_iter I>
JsonNode apply_selector(const FlattenSelector& /*unused*/, const JsonArray& arr,
                        I next, I end) {
    std::vector<JsonNode> flattened_array;

    for (const JsonNode& item : arr.get()) {
        // calculate sub result and flatten if result is an array
        const JsonNode result = apply_selector(item, next, end);
        result.apply_visitor(
            overloaded{[&flattened_array](const JsonArray& nested_array) {
                           extend_vec_with(flattened_array, nested_array.get());
                       },
                       [](const is_json_item auto& /*unused*/) {}});
    }

    return JsonNode(JsonArray(flattened_array));
}

template <sel_iter I>
JsonNode apply_selector(const TruncateSelector& /*unused*/,
                        const JsonObject& /*unused*/, I next, I end) {
    if (next != end) {
        // TODO create something better to emit warnings
        std::cerr << "Truncate is not last selector\n";
    }
    return JsonNode(JsonObject());
}

template <sel_iter I>
JsonNode apply_selector(const TruncateSelector& /*unused*/,
                        const JsonArray& /*unused*/, I next, I end) {
    if (next != end) {
        std::cerr << "Truncate is not last selector\n";
    }
    return JsonNode(JsonArray());
}

template <sel_iter I>
JsonNode apply_selector(const TruncateSelector& /*unused*/,
                        const is_json_item auto& json, I next, I end) {
    if (next != end) {
        std::cerr << "Truncate is not last selector\n";
    }
    return JsonNode(json);
}

template <sel_iter I>
JsonNode apply_selector(const FilterSelector& s, const JsonArray& arr, I next,
                        I end) {
    std::vector<JsonNode> result;

    for (const JsonNode& item : arr.get()) {
        try {
            // only check JsonObjects and ignore all other items
            item.apply_visitor(overloaded{
                [&result, &next, &end, key = s.get()](const JsonObject& obj) {
                    result.push_back(apply_selector(key, obj, next, end));
                },
                [](const is_json_item auto& /*unused*/) {}});
        } catch (const ApplySelectorError&) {
            // note: this means th key was not found and thus we ignore the item
        }
    }

    return JsonNode(JsonArray(result));
}

template <sel_iter I>
JsonNode apply_selector(const PropertySelector& s, const JsonObject& obj,
                        I next, I end) {
    const std::vector<std::string>& keys = s.get_keys();
    // initialize with correct size so we don't need back_inserter
    std::vector<std::pair<std::string, JsonNode>> result{keys.size()};

    std::ranges::transform(
        keys, result.begin(), [&obj, next, end](const std::string& key) {
            return std::make_pair(key,
                                  apply_selector(obj.find(key), next, end));
        });

    return JsonNode(JsonObject(result));
}

template <sel_iter I>
JsonNode apply_selector(const RangeSelector& s, const JsonArray& array, I next,
                        I end) {
    const std::vector<JsonNode>& arr = array.get();

    // range start and end or default values
    const auto range_start = s.get_start().get_value_or(0);
    const auto range_end = s.get_end().get_value_or(arr.size() - 1) + 1;

    auto begin_it = arr.cbegin() + range_start;
    auto end_it = arr.cbegin() + range_end;

    const unsigned long num_items = range_end - range_start;

    std::vector<JsonNode> result{num_items};
    std::transform(begin_it, end_it, result.begin(),
                   [next, end](const JsonNode& item) {
                       return apply_selector(item, next, end);
                   });

    return {JsonArray{result}};
}

template <sel_iter I>
JsonNode apply_selector(const IndexSelector& s, const JsonArray& arr, I next,
                        I end) {
    return apply_selector(arr.at(s.get()), next, end);
}

template <sel_iter I>
JsonNode apply_selector(const KeySelector& s, const JsonObject& obj, I next,
                        I end) {
    try {
        return apply_selector(obj.find(s.get()), next, end);
    } catch (std::out_of_range&) {
        throw ApplySelectorError("Key \"" + s.get() +
                                 "\" was not found in json object");
    }
}

template <sel_iter I>
JsonNode apply_selector(const AnyRootSelector& /*unused*/,
                        const is_json_item auto& json, I next, I end) {
    return apply_selector(json, next, end);
}

template <sel_iter I>
JsonNode apply_selector(const is_selector auto& s, const is_json_item auto& j,
                        I /*unused*/, I /*unused*/) {
    throw ApplySelectorError(
        std::string("selector and json object don't match: ") + s.name() +
        ", " + j.name());
}

// entry point for applying the next selector
template <sel_iter I>
JsonNode apply_selector(const JsonNode& json, I next, I end) {
    if (next == end) {
        return JsonNode(json);
    }

    const SelectorNode& next_s = *next;
    next++;
    return json.apply_visitor(
        [&next, &end](is_json_item auto& item, is_selector auto& selector) {
            return apply_selector(selector, item, next, end);
        },
        next_s.inner);
}

/**
 * Contains a list of sequential selectors.
 *
 * The result of applying the selector is the result of applying all selectors
 * in sequential order.
 */
class RootSelector {
    std::vector<SelectorNode> inner;

public:
    RootSelector() = default;
    RootSelector(std::vector<SelectorNode> inner) : inner(std::move(inner)) {}

    const std::vector<SelectorNode>& get() const { return inner; }

    JsonNode apply(const JsonNode& json) const {
        return apply_selector(json, inner.cbegin(), inner.cend());
    }

    friend std::ostream& operator<<(std::ostream& o, const RootSelector& self) {
        o << "RootSelector {";
        for (const auto& x : self.inner) {
            o << x << ",";
        }
        return o << "}";
    }
};

/**
 * Selectors is a list of independent [RootSelectors](@ref RootSelector).
 *
 * If there is only one root selector the result of applying the selectors is
 * simply the result of that selector.  If there is more than one root
 * selectors the result is a list of the results of those selectors.
 *
 * TODO decide if an empty list of root selectors is valid (and probably treat
 * is as a noop)
 */
class Selectors {
    std::vector<RootSelector> selectors;

public:
    Selectors() = default;
    Selectors(std::vector<RootSelector> selectors)
        : selectors(std::move(selectors)) {}

    const std::vector<RootSelector>& get() const { return selectors; }
    std::vector<RootSelector>& get() { return selectors; }

    /**
     * Apply all the selectors to the given Json.
     *
     * Throws ApplySelectorError if one of the selectors can't be applied to
     * the json.
     */
    JsonNode apply(const JsonNode& json) const {
        JsonNode result;

        if (selectors.size() == 1) {
            result = selectors[0].apply(json);
        } else {
            auto apply = [&json](const RootSelector& selector) {
                return selector.apply(json);
            };
            std::vector<JsonNode> array;
            ranges::transform(selectors, std::back_inserter(array), apply);
            result = JsonNode(JsonArray(array));
        }

        return result;
    }

    friend std::ostream& operator<<(std::ostream& o, const Selectors& self) {
        o << '[';
        for (const auto& x : self.selectors) {
            o << x << ',';
        }
        return o << ']';
    }
};

} // namespace selectors

#endif
