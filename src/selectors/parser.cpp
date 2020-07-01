#ifndef NDEBUG
    #undef BOOST_SPIRIT_DEBUG
#endif

#include <boost/phoenix.hpp>
#include <boost/spirit/home/qi/nonterminal/debug_handler.hpp>
#include <boost/spirit/include/qi.hpp>
#include <iterator>
#include <memory>
#include <variant>

#include "selector_types.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

class FailedToParseSelectorException : public std::exception {
  public:
    virtual const char *what() const noexcept override {
        return "Failed to parse selector";
    }
};

template <typename Iterator>
Selectors parse_selectors(Iterator first, Iterator last) {
    using boost::phoenix::construct;
    using boost::phoenix::new_;
    using boost::phoenix::val;
    using qi::_1;
    using qi::_2;
    using qi::_val;
    using qi::rule;

    rule<Iterator, std::string()> quoted_string =
        qi::no_skip[qi::lexeme['"' >> +(ascii::char_ - '"') >> '"']];

    // base selectors
    rule<Iterator, KeySelector()> key =
        quoted_string[_val = construct<KeySelector>(_1)];
    key.name("key");

    rule<Iterator, AnyRootSelector(), ascii::space_type> any_root =
        qi::lit('.')[_val = construct<AnyRootSelector>()];
    any_root.name("any_root");

    rule<Iterator, IndexSelector(), ascii::space_type> index_sel =
        ('[' >> qi::int_ >> ']')[_val = construct<IndexSelector>(_1)];
    index_sel.name("index");

    rule<Iterator, RangeSelector(), ascii::space_type> range_sel =
        ('[' >> -qi::int_ >> ':' >> -qi::int_ >>
         ']')[_val = construct<RangeSelector>(_1, _2)];
    range_sel.name("range");

    rule<Iterator, RangeSelector(), ascii::space_type> empty_range_sel =
        (qi::lit('[') >> ']')[_val = construct<RangeSelector>()];
    empty_range_sel.name("empty_range");

    rule<Iterator, PropertySelector(), ascii::space_type> property_sel =
        ('{' >> (key % ',') >> '}')[_val = construct<PropertySelector>(_1)];
    property_sel.name("property");

    rule<Iterator, TruncateSelector(), ascii::space_type> truncate_sel =
        qi::lit('!')[_val = construct<TruncateSelector>()];
    truncate_sel.name("truncate");

    // combined selectors
    rule<Iterator, FilterSelector(), ascii::space_type> raw_filter_sel =
        ('|' >> key)[_val = construct<FilterSelector>(_1)];
    raw_filter_sel.name("raw_filter");
    rule<Iterator, SelectorNode(), ascii::space_type> filter_sel =
        raw_filter_sel[_val = construct<SelectorNode>(_1)];
    filter_sel.name("filter");

    rule<Iterator, SelectorNode(), ascii::space_type> basic_sel =
        (key | index_sel | range_sel | empty_range_sel | property_sel)[bind(&SelectorNode::inner, _val) = _1];
    basic_sel.name("basic");

    rule<Iterator, SelectorNode(), ascii::space_type> compound_sel =
        -qi::lit('.') >> (basic_sel | filter_sel);
    compound_sel.name("compound");

    rule<Iterator, std::vector<SelectorNode>(), ascii::space_type> raw_root_item =
        basic_sel >> *(compound_sel);
    raw_root_item.name("raw_root_item");

    rule<Iterator, RootSelector(), ascii::space_type> root_item =
        raw_root_item[_val = construct<RootSelector>(_1)];
    root_item.name("root_item");

    rule<Iterator, Selectors(), ascii::space_type> root =
        (root_item % ',')[_val = construct<Selectors>(_1)];
    root.name("root");

    BOOST_SPIRIT_DEBUG_NODE(key);
    BOOST_SPIRIT_DEBUG_NODE(root_item);
    BOOST_SPIRIT_DEBUG_NODE(root);

    Selectors selectors;
    if (!qi::phrase_parse(first, last, root, ascii::space, selectors)) {
        throw FailedToParseSelectorException();
    }

    // std::cerr << "res: " << res << "pos: " << std::distance(first, last) <<
    // "\n";

    // fail we did not get a fill match
    if (first != last) {
        throw FailedToParseSelectorException();
    }

    return selectors;
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
//   - stops processing and returns a simple value (for objects and array an
//   empty object or array)
//
// This isn't really a selector but transforms the output:
// flatten arrays .. (prefix instead of postfix)
