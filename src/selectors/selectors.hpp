#include "selector_type.hpp"
#include "driver.hpp"
#include <sstream>

// class driver {
//     public:
//         int parse(const std::string& f) {
//             file = f;
//             location.initialize(&file);
//             scan_begin();
//             yy::parser parse(*this);
//             parse.set_debug_level(trace_parsing);
//             int res = parse();
//             scan_end();
//             return res;
//         }
//
//         std::string file;
//         bool trace_parsing = false;
//
//         void scan_begin();
//         void scan_end();
//         bool trace_scanning = false;
//         yy::location location;
//
// };

Selector parse_selector(std::string s) {
    SelectorDriver driver(s);
    RootSelector root = driver.parse();

    // if (!parse()) {
    //    root.print();
    // } else {
    //     std::cout << "Error\n";
    // }
    return root;
}


// SELECTORS:
//
// root .
// key "something"
// child "something"."else"
// index [1]
// multi-index [1,5]
// range [1:4]
//   - can be reversed [4:1]
//   - and used multiple times [4:1][3:0]
//   - start or end can be omitted [1:] [:5]
//   - ommit both to select the whole array (is a bit useless)
// property {"a", "c"}
// multi-selection "one"[2:0],"two","three"
// filter "something"|"else" (will filter an array "something": [...])
// truncate !
//   - stops processing and returns a simple value (for objects and array an empty object or array)
//
// This isn't really a selector but transforms the output:
// flatten arrays .. (prefix instead of postfix)

