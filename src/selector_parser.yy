%require "3.2"
%skeleton "lalr1.cc"
%language "c++"

%define api.token.raw
%define api.token.constructor
%define api.value.type variant

%define api.parser.class {selector_parser}
%define parse.assert

%code requires {
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <variant>

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

class driver {
    public:
        RootSelector result;
};
}

%param { driver& drv }

%code {
#include <iostream>
#include <sstream>
    // Print a list of strings.
    static auto operator<<(std::ostream& o, const std::vector<std::string>& ss) -> std::ostream& {
        o << '{';
        const char *sep = "";

        for (std::vector<std::string>::const_iterator i = ss.begin(), end = ss.end(); i != end; ++i) {
            o << sep << *i;
            sep = ", ";
        }
        // for (const auto& s: ss) {
        //     o << sep << s;
        //     sep = ", ";
        // }

        return o << '}';
    }
}
%code {
namespace yy {
    auto yylex(driver& drv) -> selector_parser::symbol_type {
        static int count = 0;

        switch (int stage = count++) {
        case 0:
          return selector_parser::make_KEY(new KeySelector("something"));
        default:
          return selector_parser::make_YYEOF();
        }
    }
}
}

%printer { yyo << $$; } <*>

%type <RootSelector> result;
%type <std::vector<Selector*>> list;

%token <AnyRootSelector*> ROOT ".";
%token <KeySelector*> KEY;
// %token <ChildSelector> CHILD;


%%

result:
  list  { std::swap($$.inner, $1); drv.result = $$; }
;

list:
  ROOT {}
| KEY { $$.push_back($1); }
;

%%

namespace yy {
    // Report an error to the user.
    auto selector_parser::error(const std::string& msg) -> void {
        std::cerr << msg << '\n';
    }
}

// int main() {
//     driver d;
//     yy::selector_parser parse(d);
//     if (!parse()) {
//         d.result.print();
//     } else {
//         std::cout << "Error\n";
//     }
// }
