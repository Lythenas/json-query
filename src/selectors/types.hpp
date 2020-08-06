#ifndef JSON_QUERY_SELCTOR_TYPE_HPP
#define JSON_QUERY_SELCTOR_TYPE_HPP

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

#include "../json/json.hpp"
#include "../utils.hpp"

namespace selectors {
using namespace json;
namespace ranges = std::ranges;

class SelectorNode;

class Selector {};

// Used to detect wrong parsing because the default constructor of the
// first variant is sometimes used.
class InvalidSelector : public Selector {
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
class AnyRootSelector : public Selector {
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
class KeySelector : public Selector {
public:
    KeySelector() = default;
    KeySelector(std::string key) : key(std::move(key)) {}

    static const char* name() { return "Key"; }

    const std::string& get() const { return key; }

    friend std::ostream& operator<<(std::ostream& o, const KeySelector& self) {
        return o << "KeySelector(" << self.key << ")";
    }

private:
    std::string key;
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
class IndexSelector : public Selector {
public:
    int index;

    IndexSelector() = default;
    IndexSelector(int index) : index(index) {}

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
class RangeSelector : public Selector {
public:
    RangeSelector() = default;
    RangeSelector(boost::optional<RangeSelector> r) {
        if (r) {
            start = r->start;
            end = r->end;
        }
    }
    RangeSelector(boost::optional<int> start, boost::optional<int> end)
        : start(start), end(end) {}

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

private:
    boost::optional<int> start;
    boost::optional<int> end;
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
class PropertySelector : public Selector {
public:
    PropertySelector() = default;
    PropertySelector(std::vector<std::string> keys) : keys(std::move(keys)) {}

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

private:
    std::vector<std::string> keys;
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
class FilterSelector : public Selector {
public:
    FilterSelector() = default;
    FilterSelector(const KeySelector& filter) : filter(filter) {}

    static const char* name() { return "Filter"; }

    const KeySelector& get() const { return filter; }

    friend std::ostream& operator<<(std::ostream& o,
                                    const FilterSelector& self) {
        return o << "FilterSelector(" << self.filter << ")";
    }

private:
    KeySelector filter;
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
class TruncateSelector : public Selector {
public:
    TruncateSelector() = default;

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
class FlattenSelector : public Selector {
public:
    FlattenSelector() = default;

    static const char* name() { return "Flatten"; }

    friend std::ostream& operator<<(std::ostream& o,
                                    const FlattenSelector& /*unused*/) {
        return o << "FlattenSelector";
    }
};

/**
 * Unifying wrapper for all selector kinds.
 *
 * This avoids having to allocate each constructor separately on the heap.
 */
class SelectorNode : public Selector {
    using InnerVariant =
        boost::variant<InvalidSelector, AnyRootSelector, KeySelector,
                       IndexSelector, RangeSelector, PropertySelector,
                       FilterSelector, TruncateSelector, FlattenSelector>;

public:
    explicit SelectorNode() = default;
    explicit SelectorNode(AnyRootSelector inner) : inner(inner) {}
    explicit SelectorNode(KeySelector inner) : inner(inner) {}
    explicit SelectorNode(IndexSelector inner) : inner(inner) {}
    explicit SelectorNode(RangeSelector inner) : inner(inner) {}
    explicit SelectorNode(PropertySelector inner) : inner(inner) {}
    explicit SelectorNode(FilterSelector inner) : inner(inner) {}
    explicit SelectorNode(TruncateSelector inner) : inner(inner) {}
    explicit SelectorNode(FlattenSelector inner) : inner(inner) {}

    const char* name() const {
        return boost::apply_visitor([](const auto& x) { return x.name(); },
                                    inner);
    }

    template <typename T> const T& as() const { return boost::get<T>(inner); }

    template <typename Iterator>
    JsonNode apply(const JsonNode& json, Iterator next, Iterator end) const {
        return json.apply_visitor(
            [next, end](const auto& o, const auto& s) {
                return s.apply(o, next, end);
            },
            inner);
    }

    friend std::ostream& operator<<(std::ostream& o, const SelectorNode& self) {
        boost::apply_visitor([&o](auto val) { o << val; }, self.inner);
        return o;
    }

    // private:
    InnerVariant inner;
};

// these template functions make it a little easier to find and extend what
// selector handles what json item
// but they generate ridiculous error matches...

template <typename I> concept as_iter = std::random_access_iterator<I>;

JsonNode apply_selector(const FlattenSelector& /*unused*/, const JsonArray& arr,
                        as_iter auto next, as_iter auto end) {
    std::vector<JsonNode> flattened_array;

    for (const auto& item : arr.get()) {
        // calculate sub result and flatten if result is array
        const JsonNode result = apply_selector(item, next, end);
        result.apply_visitor(
            overloaded{[&flattened_array](const JsonArray& nested_array) {
                           std::copy(nested_array.get().cbegin(),
                                     nested_array.get().cend(),
                                     std::back_inserter(flattened_array));
                       },
                       [](const auto& /*unused*/) {}});
    }

    return JsonNode(JsonArray(flattened_array));
}
JsonNode apply_selector(const TruncateSelector& /*unused*/,
                        const JsonObject& /*unused*/, as_iter auto next,
                        as_iter auto end) {
    if (next != end) {
        // TODO create something better to emit warnings
        std::cerr << "Truncate is not last selector\n";
    }
    return JsonNode(JsonObject());
}
JsonNode apply_selector(const TruncateSelector& /*unused*/,
                        const JsonArray& /*unused*/, as_iter auto next,
                        as_iter auto end) {
    if (next != end) {
        std::cerr << "Truncate is not last selector\n";
    }
    return JsonNode(JsonArray());
}
JsonNode apply_selector(const TruncateSelector& /*unused*/, const auto& json,
                        as_iter auto next, as_iter auto end) {
    if (next != end) {
        std::cerr << "Truncate is not last selector\n";
    }
    return JsonNode(json);
}
JsonNode apply_selector(const FilterSelector& s, const JsonArray& arr,
                        as_iter auto next, as_iter auto end) {
    std::vector<JsonNode> result;

    for (const auto& item : arr.get()) {
        try {
            // only check JsonObjects and ignore all other items
            item.apply_visitor(overloaded{
                [&result, &next, &end, key = s.get()](const JsonObject& obj) {
                    result.push_back(apply_selector(key, obj, next, end));
                },
                [](const auto& /*unused*/) {}});
        } catch (const std::out_of_range&) {
        }
    }

    return JsonNode(JsonArray(result));
}
JsonNode apply_selector(const PropertySelector& s, const JsonObject& obj,
                        as_iter auto next, as_iter auto end) {
    const auto& keys = s.get_keys();
    std::vector<std::pair<std::string, JsonNode>> result{keys.size()};

    std::transform(keys.cbegin(), keys.cend(), result.begin(),
                   [&obj, next, end](const auto& key) {
                       return std::make_pair(
                           key, apply_selector(obj.find(key), next, end));
                   });

    return {JsonObject(result)};
}
JsonNode apply_selector(const RangeSelector& s, const JsonArray& array,
                        as_iter auto next, as_iter auto end) {
    const auto& arr = array.get();
    auto begin_it = arr.cbegin() + s.get_start().get_value_or(0);
    auto end_it = arr.cbegin() + s.get_end().get_value_or(arr.size() - 1) + 1;

    const unsigned long num_items = s.get_end().get_value_or(arr.size() - 1) +
                                    1 - s.get_start().get_value_or(0);

    std::vector<JsonNode> result{num_items};
    std::transform(begin_it, end_it, result.begin(),
                   [next, end](const JsonNode& item) {
                       return apply_selector(item, next, end);
                   });

    return {JsonArray{result}};
}
JsonNode apply_selector(const IndexSelector& s, const JsonArray& arr,
                        as_iter auto next, as_iter auto end) {
    return apply_selector(arr.at(s.get()), next, end);
}
JsonNode apply_selector(const KeySelector& s, const JsonObject& obj,
                        as_iter auto next, as_iter auto end) {
    return apply_selector(obj.find(s.get()), next, end);
}
JsonNode apply_selector(const AnyRootSelector& /*unused*/, const auto& json,
                        as_iter auto next, as_iter auto end) {
    return apply_selector(json, next, end);
}
JsonNode apply_selector(const auto& s, const auto& j, as_iter auto /*unused*/,
                        as_iter auto /*unused*/) {
    throw std::runtime_error(
        std::string("selector and json object don't match: ") + s.name() +
        ", " + j.name());
}

JsonNode apply_selector(const JsonNode& json, as_iter auto next,
                        as_iter auto end) {
    if (next == end) {
        return JsonNode(json);
    }

    const SelectorNode& next_s = *next;
    // don't use ++ here because that would change the iterator and then we
    // couldn't use it in the loops for e.g. RangeSelector
    next = next + 1;
    return json.apply_visitor(
        [&next, &end](auto& item, auto& selector) {
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
class RootSelector : public Selector {
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

private:
    std::vector<SelectorNode> inner;
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
public:
    Selectors() = default;
    Selectors(std::vector<RootSelector> selectors)
        : selectors(std::move(selectors)) {}

    const std::vector<RootSelector>& get() const { return selectors; }
    std::vector<RootSelector>& get() { return selectors; }

    Json apply(const Json& json) const {
        const JsonNode& node = json.get();
        Json result{};

        if (selectors.size() == 1) {
            result.get() = selectors[0].apply(node);
        } else {
            auto apply = [&node](const auto& selector) {
                return selector.apply(node);
            };
            std::vector<JsonNode> array;
            ranges::transform(selectors, std::back_inserter(array), apply);
            result.get() = JsonNode(JsonArray(array));
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

private:
    std::vector<RootSelector> selectors;
};

} // namespace selectors

#endif
