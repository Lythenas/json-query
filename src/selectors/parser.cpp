#ifndef NDEBUG
    #undef BOOST_SPIRIT_DEBUG
#endif

#include <boost/phoenix.hpp>
#include <boost/spirit/home/qi/nonterminal/debug_handler.hpp>
#include <boost/spirit/include/qi.hpp>
#include <iterator>
#include <memory>

#include "types.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

class FailedToParseSelectorException : public std::exception {
  public:
    FailedToParseSelectorException(const char* reason): reason(reason) {
    }

    virtual const char *what() const noexcept override {
        return reason;
    }

  private:
    const char* reason;
};

template<typename Iterator>
struct selectors_grammar : qi::grammar<Iterator, Selectors(), ascii::space_type> {
    selectors_grammar() : selectors_grammar::base_type(root) {
        using boost::phoenix::construct;
        using boost::phoenix::new_;
        using boost::phoenix::val;
        using qi::_1;
        using qi::_2;
        using qi::_val;
        using qi::rule;

        root = (root_item % ',')[_val = construct<Selectors>(_1)];

        root_item = raw_root_item[_val = construct<RootSelector>(_1)];
        raw_root_item = basic >> *(compound);

        compound = -qi::lit('.') >> basic;
        basic = (any_root | key | index | range | property | truncate | filter)[bind(&SelectorNode::inner, _val) = _1];

        filter = ('|' >> key)[_val = construct<FilterSelector>(_1)];
        truncate = qi::lit('!')[_val = construct<TruncateSelector>()];
        property = ('{' >> (key % ',') >> '}')[_val = construct<PropertySelector>(_1)];
        inner_range = (-qi::int_ >> ':' >> -qi::int_)[_val = construct<RangeSelector>(_1, _2)];
        range = ('[' >> -inner_range >> ']')[_val = construct<RangeSelector>(_1)];
        index = ('[' >> qi::int_ >> ']')[_val = construct<IndexSelector>(_1)];
        any_root = qi::lit('.')[_val = construct<AnyRootSelector>()];
        key = quoted_string[_val = construct<KeySelector>(_1)];

        quoted_string = qi::no_skip[qi::lexeme['"' >> *(ascii::char_ - '"') >> '"']];

        // set names and add debugging (ifndef NDEBUG)
        BOOST_SPIRIT_DEBUG_NODE(root);

        BOOST_SPIRIT_DEBUG_NODE(root_item);
        BOOST_SPIRIT_DEBUG_NODE(raw_root_item);

        BOOST_SPIRIT_DEBUG_NODE(compound);
        BOOST_SPIRIT_DEBUG_NODE(basic);

        BOOST_SPIRIT_DEBUG_NODE(filter);
        BOOST_SPIRIT_DEBUG_NODE(truncate);
        BOOST_SPIRIT_DEBUG_NODE(property);
        BOOST_SPIRIT_DEBUG_NODE(inner_range);
        BOOST_SPIRIT_DEBUG_NODE(range);
        BOOST_SPIRIT_DEBUG_NODE(index);
        BOOST_SPIRIT_DEBUG_NODE(any_root);
        BOOST_SPIRIT_DEBUG_NODE(key);
        BOOST_SPIRIT_DEBUG_NODE(quoted_string);
    }

    qi::rule<Iterator, Selectors(), ascii::space_type> root;
    qi::rule<Iterator, RootSelector(), ascii::space_type> root_item;
    qi::rule<Iterator, std::vector<SelectorNode>(), ascii::space_type> raw_root_item;

    qi::rule<Iterator, SelectorNode(), ascii::space_type> compound;
    qi::rule<Iterator, SelectorNode(), ascii::space_type> basic;

    qi::rule<Iterator, FilterSelector(), ascii::space_type> filter;
    qi::rule<Iterator, TruncateSelector(), ascii::space_type> truncate;
    qi::rule<Iterator, PropertySelector(), ascii::space_type> property;
    qi::rule<Iterator, RangeSelector(), ascii::space_type> inner_range;
    qi::rule<Iterator, RangeSelector(), ascii::space_type> range;
    qi::rule<Iterator, IndexSelector(), ascii::space_type> index;
    qi::rule<Iterator, AnyRootSelector()> any_root;
    qi::rule<Iterator, KeySelector()> key;

    qi::rule<Iterator, std::string()> quoted_string;
};

template <typename Iterator>
Selectors parse_selectors(Iterator first, Iterator last) {
    Selectors selectors;
    if (!qi::phrase_parse(first, last, selectors_grammar<Iterator>(), ascii::space, selectors)) {
        throw FailedToParseSelectorException("parser failed");
    }

    // fail if we did not get a full match
    if (first != last) {
        throw FailedToParseSelectorException("didn't consume all input");
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
