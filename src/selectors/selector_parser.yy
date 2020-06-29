%require "3.6.4"
%skeleton "lalr1.cc"
%language "c++"
%defines

%define api.token.raw
%define api.token.constructor
%define api.value.type variant

%define api.parser.class {selector_parser}
%define parse.assert
// %define parse.trace

%code requires {
    #include <iostream>
    #include <memory>
    #include <string>
    #include <vector>
    #include <variant>

    #include "selector_type.hpp"

    // declare driver class (later included)
    // we can't include here because of cyclic dependency
    class SelectorDriver;
}

%param { SelectorDriver& drv }
%parse-param { RootSelector& root }

%define parse.error detailed

%code {
    #include <iostream>
    #include <sstream>

    #include "driver.hpp"

    static auto operator<<(std::ostream& o, const std::vector<std::string>& ss) -> std::ostream& {
        const char *sep = "";

        o << '{';

        for (std::vector<std::string>::const_iterator i = ss.begin(), end = ss.end(); i != end; ++i) {
            o << sep << *i;
            sep = ", ";
        }

        return o << '}';
    }

    /* namespace yy { */
    /*     auto yylex(Lexer& lexer) -> selector_parser::symbol_type { */
    /*         return lexer.next(); */
    /*     } */
    /* } */
}

%printer { yyo << $$; } <*>

%type <RootSelector> result;
%type <std::vector<Selector*>> list;

%token
    DOT "."
    <std::string> KEY
;

// %token <AnyRootSelector*> ROOT ".";
// %token <KeySelector*> KEY;
// %token <ChildSelector> CHILD;


%%

result:
  list  { std::swap($$.inner, $1); root = $$; }
;

list:
  DOT {}
| KEY { $$.push_back(new KeySelector($1)); }
;

%%

// Report an error to the user.
auto yy::selector_parser::error(const std::string& msg) -> void {
    std::cerr << msg << '\n';
}

