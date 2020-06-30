#ifndef JSON_QUERY_SELCTOR_TYPE_HPP
#define JSON_QUERY_SELCTOR_TYPE_HPP

#include <boost/optional.hpp>
#include <iostream>
#include <vector>

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

class RootSelector : public Selector {
  public:
    std::vector<Selector *> inner;

    virtual ~RootSelector() noexcept {
        for (auto &x : inner) {
            delete x;
        }
    }

    virtual void print() const override {
        std::cout << "RootSelector {";
        for (const auto &x : inner) {
            x->print();
            std::cout << ",";
        }
        std::cout << "}\n";
    }

    // TODO
    virtual Json *run(Json *json) const override {
        // zero inner selectors are invalid
        assert(inner.size() > 0);

        if (inner.size() > 1) {
            for (auto &selector : inner) {
                // run selectors
            }
            // return list of results
            return new Json();
        } else {
            return inner[0]->run(json);
        }
    }
};

class AnyRootSelector : public Selector {
  public:
    virtual ~AnyRootSelector() noexcept override = default;

    virtual void print() const override { std::cout << "AnyRootSelector"; }

    virtual Json *run(Json *json) const override { return json; }
};

class KeySelector : public Selector {
  public:
    KeySelector() = default;
    KeySelector(std::string key) : key(key) {}
    virtual ~KeySelector() noexcept = default;

    virtual void print() const noexcept override {
        std::cout << "KeySelector(\"" << key << "\")";
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

    IndexSelector(int index) : index(index) {}
    virtual ~IndexSelector() noexcept override = default;

    virtual void print() const noexcept override {
        std::cout << "IndexSelector(" << index << ")";
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
    virtual ~RangeSelector() noexcept = default;

    virtual void print() const noexcept override {
        std::cout << "RangeSelector("
                  << (start.has_value() ? std::to_string(start.value()) : "")
                  << "," << (end.has_value() ? std::to_string(end.value()) : "")
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

class ChildSelector : public Selector {
  public:
    ChildSelector() = default;
    ChildSelector(Selector *child) : child(child) {}
    virtual ~ChildSelector() noexcept { delete child; }

    virtual void print() const noexcept override {
        std::cout << "ChildSelector(";
        child->print();
        std::cout << ")";
    }

    // TODO
    virtual Json *run(Json *json) const override {
        // same as KeySelector
        // select one key from an object
        // error if not an object
        return new Json();
    }

  private:
    Selector *child;
};

class PropertySelector : public Selector {
  public:
    PropertySelector() = default;
    PropertySelector(std::vector<KeySelector *> keys) : keys(keys) {}
    virtual ~PropertySelector() noexcept {
        for (auto &x : keys) {
            delete x;
        }
    }

    virtual void print() const noexcept override {
        std::cout << "PropertySelector{";
        for (auto &x : keys) {
            x->print();
            std::cout << ",";
        }
        std::cout << "}";
    }

    // TODO
    virtual Json *run(Json *json) const override {
        // select properties from object
        // and return a new object with those properties
        // error if not an object
        return new Json();
    }

  private:
    std::vector<KeySelector *> keys;
};

class FilterSelector : public Selector {
  public:
    FilterSelector() = default;
    FilterSelector(KeySelector *filter) : filter(filter) {}
    virtual ~FilterSelector() noexcept { delete filter; }

    virtual void print() const noexcept override {
        std::cout << "FilterSelector(";
        filter->print();
        std::cout << "}";
    }

    // TODO
    virtual Json *run(Json *json) const override {
        // filter array for elements with that key, select that key
        // and return new array
        // error if not an array
        return new Json();
    }

  private:
    KeySelector *filter;
};

class TruncateSelector : public Selector {
  public:
    TruncateSelector() = default;
    virtual ~TruncateSelector() = default;

    virtual void print() const noexcept override {
        std::cout << "TruncateSelector";
    }

    // TODO
    virtual Json *run(Json *json) const override {
        // stop processing json
        // and return the trivial element or an empty array or object
        return new Json();
    }
};

#endif
