#ifndef SELECTOR_DRIVER_HPP
#define SELECTOR_DRIVER_HPP

#include <string>
#include <map>

#include "selector_parser.hpp"

#define YY_DECL \
    yy::parser::symbol_type yylex (driver& drv)

class driver {
    public:
        int parse(const std::string& f) {
            file = f;
            location.initialize(&file);
            scan_begin();
            yy::parser parse(*this);
            parse.set_debug_level(trace_parsing);
            int res = parse();
            scan_end();
            return res;
        }

        std::string file;
        bool trace_parsing = false;

        void scan_begin();
        void scan_end();
        bool trace_scanning = false;
        yy::location location;

};

YY_DECL;

#endif
