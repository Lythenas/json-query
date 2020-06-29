#ifndef JSON_QUERY_SELCTOR_TYPE_HPP
#define JSON_QUERY_SELCTOR_TYPE_HPP

#include <iostream>
#include <vector>

class Selector {
    public:
        virtual void print() const { std::cout << "unimplemented!"; };
};

class RootSelector: public Selector {
    public:
        std::vector<Selector*> inner;

        virtual void print() const override {
            std::cout << "RootSelector {";
            for (const auto& x : inner) {
                x->print();
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

        virtual void print() const override {
            std::cout << "KeySelector(\"" << key << "\")";
        }
};


#endif
