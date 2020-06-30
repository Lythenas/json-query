#ifndef JSON_QUERY_SELCTOR_TYPE_HPP
#define JSON_QUERY_SELCTOR_TYPE_HPP

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <iostream>
#include <vector>
#include <variant>

class Json {};

/**
 *  Selectors are linear (except for the selectors in RootSelector).
 *
 *  That means selectors aren't a tree and can't be grouped. They are executed
 *  from left to right.
 */

class Selector {
  public:
    virtual ~Selector() noexcept = default;
    virtual void print() const { std::cout << "unimplemented!"; };
    virtual Json *run(Json *) const = 0;
};

class AnyRootSelector : public Selector {
  public:
    ~AnyRootSelector() = default;

    virtual void print() const override { std::cout << "AnyRootSelector"; }

    virtual Json *run(Json *json) const override { return json; }
};

class KeySelector : public Selector {
  public:
    KeySelector() = default;
    KeySelector(std::string key) : key(key) {}
    ~KeySelector() = default;

    virtual void print() const noexcept override {
        std::cout << "KeySelector(\"" << key << "\")";
    }

    friend std::ostream& operator<<(std::ostream& o, const KeySelector& self) {
        return o << "KeySelector(" << self.key << ")";
    }

    // TODO
    virtual Json *run(Json *json) const override {
        // selects one key from an object
        // error if not an object
        return new Json();
    }

  private:
    std::string key;
};

class IndexSelector : public Selector {
  public:
    int index;

    IndexSelector() = default;
    IndexSelector(int index) : index(index) {}
    ~IndexSelector() = default;

    virtual void print() const noexcept override {
        std::cout << "IndexSelector(" << index << ")";
    }

    friend std::ostream& operator<<(std::ostream& o, const IndexSelector& self) {
        return o << "IndexSelector(" << self.index << ")";
    }

    // TODO
    virtual Json *run(Json *json) const override {
        // select one element from an array
        // error if not an array
        return new Json();
    }
};

class RangeSelector : public Selector {
  public:
    RangeSelector() = default;
    RangeSelector(boost::optional<int> start, boost::optional<int> end)
        : start(start), end(end) {}
    ~RangeSelector() = default;

    virtual void print() const noexcept override {
        std::cout << "RangeSelector("
                  << (start.has_value() ? std::to_string(start.value()) : "")
                  << "," << (end.has_value() ? std::to_string(end.value()) : "")
                  << ")";
    }

    friend std::ostream& operator<<(std::ostream& o, const RangeSelector& self) {
        return o << "RangeSelector("
                  << (self.start.has_value() ? std::to_string(self.start.value()) : "")
                  << "," << (self.end.has_value() ? std::to_string(self.end.value()) : "")
                  << ")";
    }

    // TODO
    virtual Json *run(Json *json) const override {
        // select one element from an array
        // error if not an array
        return new Json();
    }

  private:
    boost::optional<int> start;
    boost::optional<int> end;
};

class PropertySelector : public Selector {
  public:
    PropertySelector() = default;
    PropertySelector(std::vector<KeySelector> keys) : keys(keys) {}
    ~PropertySelector() = default;
    // virtual ~PropertySelector() noexcept {
    //     for (auto &x : keys) {
    //         delete x;
    //     }
    // }

    virtual void print() const noexcept override {
        std::cout << "PropertySelector{";
        for (auto &x : keys) {
            x.print();
            std::cout << ",";
        }
        std::cout << "}";
    }

    friend std::ostream& operator<<(std::ostream& o, const PropertySelector& self) {
        o << "PropertySelector(";
        for (auto &x: self.keys) {
            o << x << ",";
        }
        return o << ")";
    }

    // TODO
    virtual Json *run(Json *json) const override {
        // select properties from object
        // and return a new object with those properties
        // error if not an object
        return new Json();
    }

  private:
    std::vector<KeySelector> keys;
};

class FilterSelector : public Selector {
  public:
    FilterSelector() = default;
    FilterSelector(KeySelector filter) : filter(filter) {}
    ~FilterSelector() = default;
    // virtual ~FilterSelector() noexcept { delete filter; }

    virtual void print() const noexcept override {
        std::cout << "FilterSelector(";
        filter.print();
        std::cout << "}";
    }

    friend std::ostream& operator<<(std::ostream& o, const FilterSelector& self) {
        return o << "FilterSelector(" << self.filter << ")";
    }

    // TODO
    virtual Json *run(Json *json) const override {
        // filter array for elements with that key, select that key
        // and return new array
        // error if not an array
        return new Json();
    }

  private:
    KeySelector filter;
};

class TruncateSelector : public Selector {
  public:
    TruncateSelector() = default;
    ~TruncateSelector() = default;

    virtual void print() const noexcept override {
        std::cout << "TruncateSelector";
    }

    friend std::ostream& operator<<(std::ostream& o, const TruncateSelector&) {
        return o << "TruncateSelector";
    }

    // TODO
    virtual Json *run(Json *json) const override {
        // stop processing json
        // and return the trivial element or an empty array or object
        return new Json();
    }
};

struct print_visitor : public boost::static_visitor<> {
    template<typename T>
    void operator()(T& operand) const {
        operand.print();
    }
};

struct SelectorNode {
    // enum{ANY,KEY,INDEX,RANGE,PROPERTY,FILTER,TRUNCATE};
    // union {
    //     AnyRootSelector any;
    //     KeySelector key;
    //     IndexSelector index;
    //     RangeSelector range;
    //     PropertySelector property;
    //     FilterSelector filter;
    //     TruncateSelector truncate;
    // };
    SelectorNode() = default;
    SelectorNode(AnyRootSelector inner): inner(inner) {
    }
    SelectorNode(KeySelector inner): inner(inner) {
    }
    SelectorNode(IndexSelector inner): inner(inner) {
    }
    SelectorNode(RangeSelector inner): inner(inner) {
    }
    SelectorNode(PropertySelector inner): inner(inner) {
    }
    SelectorNode(FilterSelector inner): inner(inner) {
    }
    SelectorNode(TruncateSelector inner): inner(inner) {
    }

    boost::variant<KeySelector, IndexSelector, RangeSelector, PropertySelector, FilterSelector, AnyRootSelector, TruncateSelector> inner;

    void print() const {
        // std::cout << inner.which() << " x ";
        boost::apply_visitor(print_visitor(), inner);
        // if (std::holds_alternative<AnyRootSelector>(inner)) {
        //     std::get<AnyRootSelector>(inner).print();
        // } else if (std::holds_alternative<KeySelector>(inner)) {
        //     std::get<KeySelector>(inner).print();
        // } else if (std::holds_alternative<IndexSelector>(inner)) {
        //     std::get<IndexSelector>(inner).print();
        // } else if (std::holds_alternative<RangeSelector>(inner)) {
        //     std::get<RangeSelector>(inner).print();
        // } else if (std::holds_alternative<PropertySelector>(inner)) {
        //     std::get<PropertySelector>(inner).print();
        // } else if (std::holds_alternative<FilterSelector>(inner)) {
        //     std::get<FilterSelector>(inner).print();
        // } else if (std::holds_alternative<TruncateSelector>(inner)) {
        //     std::get<TruncateSelector>(inner).print();
        // }
    }

    friend std::ostream& operator<<(std::ostream& o, const SelectorNode& self) {
        if (const KeySelector* x = boost::get<KeySelector>(&self.inner)) {
            return o << *x;
        } else if (const IndexSelector* x = boost::get<IndexSelector>(&self.inner)) {
            return o << *x;
        } else if (const RangeSelector* x = boost::get<RangeSelector>(&self.inner)) {
            return o << *x;
        } else if (const PropertySelector* x = boost::get<PropertySelector>(&self.inner)) {
            return o << *x;
        } else if (const FilterSelector* x = boost::get<FilterSelector>(&self.inner)) {
            return o << *x;
        } else if (const TruncateSelector* x = boost::get<TruncateSelector>(&self.inner)) {
            return o << *x;
        }
        return o;
    }
};

class RootSelector {
  public:
    RootSelector() = default;
    RootSelector(std::vector<SelectorNode> inner): inner(inner) {
    }

    void print() const {
        std::cout << "RootSelector {";
        for (const auto &x : inner) {
            x.print();
            std::cout << ",";
        }
        std::cout << "}\n";
    }

    friend std::ostream& operator<<(std::ostream& o, const RootSelector& self) {
        o << "RootSelector {";
        for (const auto &x : self.inner) {
            o << x << ",";
        }
        return o << "}";
    }

    // TODO
    // Json *run(Json *json) const {
    //     // zero inner selectors are invalid
    //     assert(inner.size() > 0);
    //
    //     if (inner.size() > 1) {
    //         for (auto &selector : inner) {
    //             // run selectors
    //         }
    //         // return list of results
    //         return new Json();
    //     } else {
    //         return inner[0].run(json);
    //     }
    // }
  private:
    std::vector<SelectorNode> inner;
};

class Selectors {
    public:
        Selectors() = default;
        Selectors(std::vector<RootSelector> selectors): selectors(selectors) {
        }

        const std::vector<RootSelector>& get() const {
            return selectors;
        }

        friend std::ostream& operator<<(std::ostream& o, const Selectors& self) {
            o << '[';
            for (auto& x: self.selectors) {
                o << x << ',';
            }
            return o << ']';
        }

    private:
        std::vector<RootSelector> selectors;
};

#endif
