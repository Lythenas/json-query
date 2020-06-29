#ifndef JSON_QUERY_SELECTOR_DRIVER_HPP
#define JSON_QUERY_SELECTOR_DRIVER_HPP

#include <string>
#include "selector_parser.hpp"
#include "selector_type.hpp"

#define YY_DECL yy::selector_parser::symbol_type yylex(SelectorDriver& drv)
YY_DECL;

class SelectorDriver {
    public:
        SelectorDriver(std::string& input): input(new char[input.size() + 1]), size(input.size()) {
            std::copy(input.begin(), input.end(), this->input);
            this->input[size] = '\0';
        }

        void scan_begin();
        void scan_end();

        RootSelector parse() {
            scan_begin();
            RootSelector root;
            yy::selector_parser parse(*this, root);
            if (!parse()) {
                std::cout << "ERROR!\n";
            }
            scan_end();
            return root;
        }

        // yy::location location;
    private:
        char* input;
        std::size_t size;
};

#endif
