#ifndef JSON_QUERY_SELCTOR_TYPE_HPP
#define JSON_QUERY_SELCTOR_TYPE_HPP

#include <iostream>
#include <vector>

class Selector {
    public:
        virtual ~Selector() noexcept {};
        virtual void print() const { std::cout << "unimplemented!"; };
};

class RootSelector: public Selector {
    public:
        std::vector<Selector*> inner;

        virtual ~RootSelector() noexcept override {
            for (auto& x : inner) {
                delete x;
            }
        }

        virtual void print() const override {
            std::cout << "RootSelector {";
            for (const auto& x : inner) {
                x->print();
                std::cout << ",";
            }
            std::cout << "}\n";
        }
};

class AnyRootSelector: public Selector {
    public:
        virtual void print() const override {
            std::cout << "AnyRootSelector";
        }
};

class KeySelector: public Selector {
    public:
        std::string key;
        KeySelector(): key() {}
        KeySelector(std::string key): key(key) {}
        virtual ~KeySelector() noexcept override {}

        virtual void print() const noexcept override {
            std::cout << "KeySelector(\"" << key << "\")";
        }
};

class IndexSelector: public Selector {
    public:
        int index;

        IndexSelector(int index): index(index) {
        }
        virtual ~IndexSelector() noexcept override {}

        virtual void print() const noexcept override {
            std::cout << "IndexSelector(" << index << ")";
        }
};

#endif
