#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>
#include <memory>
#include <variant>

#include "selector_types.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

class FailedToParseSelectorException: public std::exception {
    public:
        virtual const char* what() const noexcept override {
            return "Failed to parse selector";
        }
};

template <typename Iterator>
RootSelector* parse_selector(Iterator first, Iterator last) {
    using boost::phoenix::construct;
    using boost::phoenix::new_;
    using qi::rule;
    using qi::_val;
    using qi::_1;

    int count = 0;

    auto print = [&count](double const& x) { std::cout << count << ": " << x << std::endl; count++; };

    rule<Iterator, std::string()> quoted_string = qi::lexeme['"' >> +(ascii::char_ - '"') >> '"'];

    rule<Iterator, KeySelector*()> key = quoted_string[_val = new_<KeySelector>(_1)];
    rule<Iterator, AnyRootSelector*()> any_root = qi::lit('.')[_val = new_<AnyRootSelector>()];
    rule<Iterator, IndexSelector*()> index_sel = ('[' >> qi::int_ >> ']')[_val = new_<IndexSelector>(_1)];

    rule<Iterator, Selector*()> root_elems = key | any_root | index_sel;

    RootSelector* root = new RootSelector();

    bool res = qi::phrase_parse(
            first,
            last,
            root_elems % ',',
            ascii::space,
            root->inner);

    // fail we did not get a fill match
    if (first != last) {
        throw FailedToParseSelectorException();
    }

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

