#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>
#include <iterator>
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
    using qi::_2;

    rule<Iterator, std::string()> quoted_string = qi::no_skip[qi::lexeme['"' >> +(ascii::char_ - '"') >> '"']];

    // base selectors
    rule<Iterator, KeySelector*()> key = quoted_string[_val = new_<KeySelector>(_1)];
    key.name("key");

    rule<Iterator, AnyRootSelector*(), ascii::space_type> any_root = qi::lit('.')[_val = new_<AnyRootSelector>()];
    any_root.name("any_root");

    rule<Iterator, IndexSelector*(), ascii::space_type> index_sel = ('[' >> qi::int_ >> ']')[_val = new_<IndexSelector>(_1)];
    index_sel.name("index");

    rule<Iterator, RangeSelector*(), ascii::space_type> range_sel = ('[' >> -qi::int_ >> ':' >> -qi::int_ >> ']')[_val = new_<RangeSelector>(_1, _2)];
    range_sel.name("range");

    rule<Iterator, RangeSelector*(), ascii::space_type> empty_range_sel = (qi::lit('[') >> ']')[_val = new_<RangeSelector>()];
    empty_range_sel.name("empty_range");

    rule<Iterator, PropertySelector*(), ascii::space_type> property_sel = ('{' >> (key % ',') >> '}')[_val = new_<PropertySelector>(_1)];
    property_sel.name("property");

    rule<Iterator, TruncateSelector*(), ascii::space_type> truncate_sel = qi::lit('!')[_val = new_<TruncateSelector>()];
    truncate_sel.name("truncate");

    // combined selectors
    rule<Iterator, FilterSelector*(), ascii::space_type> filter_sel = ('|' >> key)[_val = new_<FilterSelector>(_1)];
    filter_sel.name("filter");

    rule<Iterator, ChildSelector*(), ascii::space_type> child_sel = ('.' >> key)[_val = new_<ChildSelector>(_1)];
    child_sel.name("child");

    rule<Iterator, Selector*(), ascii::space_type> basic_sel = key | index_sel | range_sel | empty_range_sel | property_sel;
    basic_sel.name("basic");
    rule<Iterator, Selector*(), ascii::space_type> compound_sel = basic_sel | filter_sel | child_sel;
    compound_sel.name("compound");

    rule<Iterator, std::vector<Selector*>(), ascii::space_type> root_elems = (any_root | basic_sel) >> +compound_sel;
    root_elems.name("root");

    RootSelector* root = new RootSelector();
    if (!qi::phrase_parse(
            first,
            last,
            (root_elems % ','),
            ascii::space,
            root->inner)) {
        throw FailedToParseSelectorException();
    }

    // std::cerr << "res: " << res << "pos: " << std::distance(first, last) << "\n";

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

