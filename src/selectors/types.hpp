#ifndef JSON_QUERY_SELCTOR_TYPE_HPP
#define JSON_QUERY_SELCTOR_TYPE_HPP

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <iostream>
#include <utility>
#include <vector>

#include "../json/json.hpp"
#include "../utils.hpp"

namespace selectors {
    using namespace json;
    namespace ranges = std::ranges;


    class Selector {
        public:
            // JsonNode apply(const JsonNode& json) const;
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

        static JsonNode apply(const JsonNode& json)  {
            return {json};
        }

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

        const std::string& get() const { return key; }

        JsonNode apply(const JsonNode& json) const {
            const auto& obj = json.as<JsonObject>();
            return {obj.find(key)};
        }

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

        int get() const { return index; }

        JsonNode apply(const JsonNode& json) const {
            const auto& arr = json.as<JsonArray>();
            return {arr.at(index)};
        }

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

        const boost::optional<int>& get_start() const { return start; }

        const boost::optional<int>& get_end() const { return end; }

        JsonNode apply(const JsonNode& json) const {
            const auto& arr = json.as<JsonArray>().get();

            auto begin_it = arr.cbegin() + start.get_value_or(0);
            auto end_it = arr.cbegin() + end.get_value_or(arr.size() - 1) + 1;
            std::vector<JsonNode> result{begin_it, end_it};

            return {JsonArray{result}};
        }

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

        const std::vector<std::string>& get_keys() const { return keys; }

        JsonNode apply(const JsonNode& json) const {
            const auto& obj = json.as<JsonObject>();
            std::vector<std::pair<std::string, JsonNode>> result;

            for (const auto & key : keys) {
                result.push_back(std::make_pair(key, obj.find(key)));
            }

            return {JsonObject(result)};
        }

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

        const KeySelector& get() const { return filter; }

        JsonNode apply(const JsonNode& json) const {
            const auto& arr = json.as<JsonArray>().get();
            std::vector<JsonNode> result;

            for (const auto& item : arr) {
                try {
                    auto inner = filter.apply(item);
                    result.push_back(inner);
                } catch (const std::out_of_range&) {
                } catch (const boost::bad_get&) {
                }
            }

            return {JsonArray(result)};
        }

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

        static JsonNode apply(const JsonNode& json)  {
            // replace objects and arrays by their empty value,
            // keep all other (primitive) values the same
            return json.apply_visitor(overloaded {
                [](const JsonObject& /*unused*/) { return JsonNode(JsonObject()); },
                [](const JsonArray& /*unused*/) { return JsonNode(JsonArray()); },
                [](const auto& x) { return JsonNode(x); },
            });
        }

        friend std::ostream& operator<<(std::ostream& o,
                                        const TruncateSelector& /*unused*/) {
            return o << "TruncateSelector";
        }
    };

    /**
     * Unifying wrapper for all selector kinds.
     *
     * This avoids having to allocate each constructor separately on the heap.
     */
    class SelectorNode : public Selector {
        using InnerVariant =
            boost::variant<KeySelector, IndexSelector, RangeSelector,
                           PropertySelector, FilterSelector, AnyRootSelector,
                           TruncateSelector>;

       public:
        SelectorNode() = default;
        SelectorNode(AnyRootSelector inner) : inner(inner) {}
        SelectorNode(KeySelector inner) : inner(inner) {}
        SelectorNode(IndexSelector inner) : inner(inner) {}
        SelectorNode(RangeSelector inner) : inner(inner) {}
        SelectorNode(PropertySelector inner) : inner(inner) {}
        SelectorNode(FilterSelector inner) : inner(inner) {}
        SelectorNode(TruncateSelector inner) : inner(inner) {}
        SelectorNode(InnerVariant inner) : inner(std::move(inner)) {}

        template <typename T>
        const T& as() const {
            return boost::get<T>(inner);
        }

        JsonNode apply(const JsonNode& json) const {
            auto visitor = [&json](auto& selector) {
                return selector.apply(json);
            };
            return boost::apply_visitor(visitor, inner);
        }

        friend std::ostream& operator<<(std::ostream& o, const SelectorNode& self) {
            boost::apply_visitor([&o](auto val){ o << val; }, self.inner);
            return o;
        }

       private:
        InnerVariant inner;
    };

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
            auto f = [](const auto& node, const auto & selector) {
                return selector.apply(node);
            };
            return std::accumulate(inner.begin(), inner.end(), json, f);
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
