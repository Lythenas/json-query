#ifndef JSON_QUERY_SELCTOR_TYPE_HPP
#define JSON_QUERY_SELCTOR_TYPE_HPP

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <iostream>
#include <utility>
#include <vector>

#include "../json/types.hpp"

namespace selectors {

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
       public:
        KeySelector() = default;
        KeySelector(std::string key) : key(std::move(key)) {}

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
    class IndexSelector {
       public:
        int index;

        IndexSelector() = default;
        IndexSelector(int index) : index(index) {}

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
    class PropertySelector {
       public:
        PropertySelector() = default;
        PropertySelector(std::vector<KeySelector> keys) : keys(std::move(keys)) {}

        const std::vector<KeySelector>& get_keys() const { return keys; }

        friend std::ostream& operator<<(std::ostream& o,
                                        const PropertySelector& self) {
            o << "PropertySelector(";
            for (const auto& x : self.keys) {
                o << x << ",";
            }
            return o << ")";
        }

       private:
        std::vector<KeySelector> keys;
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
       public:
        FilterSelector() = default;
        FilterSelector(const KeySelector& filter) : filter(filter) {}

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
    class TruncateSelector {
       public:
        TruncateSelector() = default;

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
    class SelectorNode {
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

        friend std::ostream& operator<<(std::ostream& o, const SelectorNode& self) {
            if (const KeySelector* x = boost::get<KeySelector>(&self.inner)) {
                return o << *x;
            } else if (const IndexSelector* x =
                           boost::get<IndexSelector>(&self.inner)) {
                return o << *x;
            } else if (const RangeSelector* x =
                           boost::get<RangeSelector>(&self.inner)) {
                return o << *x;
            } else if (const PropertySelector* x =
                           boost::get<PropertySelector>(&self.inner)) {
                return o << *x;
            } else if (const FilterSelector* x =
                           boost::get<FilterSelector>(&self.inner)) {
                return o << *x;
            } else if (const TruncateSelector* x =
                           boost::get<TruncateSelector>(&self.inner)) {
                return o << *x;
            }
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
    class RootSelector {
       public:
        RootSelector() = default;
        RootSelector(std::vector<SelectorNode> inner) : inner(std::move(inner)) {}

        const std::vector<SelectorNode>& get() const { return inner; }

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
