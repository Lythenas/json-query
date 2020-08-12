#ifndef JSON_QUERY_SELECTOR_PARSER_HPP
#define JSON_QUERY_SELECTOR_PARSER_HPP

#include <sstream>
#ifdef TRACE
#define BOOST_SPIRIT_DEBUG
#endif

#include <boost/phoenix.hpp>
#include <boost/spirit/home/qi/nonterminal/debug_handler.hpp>
#include <boost/spirit/include/qi.hpp>
#include <iterator>
#include <memory>
#include <utility>

#include "../errors.hpp"
#include "types.hpp"

namespace selectors {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

class SyntaxError : public errors::SyntaxError {
    using errors::SyntaxError::SyntaxError;
};

class FailedToParseSelectorException : public std::exception {
    const char* reason;

public:
    FailedToParseSelectorException(const char* reason) : reason(reason) {}

    virtual const char* what() const noexcept override { return reason; }
};

template <typename Iterator>
struct selectors_grammar
    : qi::grammar<Iterator, Selectors(), ascii::space_type> {
    selectors_grammar() : selectors_grammar::base_type(root, "selectors") {
        using boost::phoenix::construct;
        using boost::phoenix::if_;
        using boost::phoenix::new_;
        using boost::phoenix::throw_;
        using boost::phoenix::val;
        using qi::_1;
        using qi::_2;
        using qi::_3;
        using qi::_4;
        using qi::_val;
        using qi::fail;
        using qi::on_error;
        using qi::on_success;
        using qi::rule;

        root = (root_item % ',')[_val = construct<Selectors>(_1)];

        root_item = raw_root_item[_val = construct<RootSelector>(_1)];
        raw_root_item = basic >> *(compound);

        // flatten needs to be handles specially because a single dot already
        // means something else
        compound = flatten[_val = construct<SelectorNode>(_1)] |
                   (-qi::lit('.') >> basic)[_val = _1];
        basic = index_or_range[_val = _1] |
                flatten[_val = construct<SelectorNode>(
                            _1)] // has to be before any_root
                | any_root[_val = construct<SelectorNode>(_1)] |
                key[_val = construct<SelectorNode>(_1)] |
                property[_val = construct<SelectorNode>(_1)] |
                truncate[_val = construct<SelectorNode>(_1)] |
                filter[_val = construct<SelectorNode>(_1)];

        flatten = qi::lit("..")[_val = construct<FlattenSelector>()];
        filter = ('|' > key)[_val = construct<FilterSelector>(_1)];
        truncate = ('!' > qi::eoi)[_val = construct<TruncateSelector>()];
        property = ('{' > (quoted_string % ',') >
                    '}')[_val = construct<PropertySelector>(_1)];

        // parsed together for better performance and to prevent backtracking
        // NOTE: it's only an index if we got an int and no colon if the second
        // int was matched the colon otherwise it's a (possibly empty) range
        // was also matched or there is a parse error
        index_or_range =
            qi::lit('[') >
            (-qi::int_ > -qi::char_(':') >
             -qi::int_)[if_(_1 && !_2)[_val = construct<SelectorNode>(
                                           construct<IndexSelector>(*_1))]
                            .else_[_val = construct<SelectorNode>(
                                       construct<RangeSelector>(_1, _3))]] >
            qi::lit(']');

        any_root = qi::lit('.')[_val = construct<AnyRootSelector>()];
        key = quoted_string[_val = construct<KeySelector>(_1)];

        quoted_string =
            qi::no_skip[qi::lexeme['"' > *(ascii::char_ - '"') > '"']];

        // set names and add debugging (ifndef NDEBUG)
        BOOST_SPIRIT_DEBUG_NODE(root);

        BOOST_SPIRIT_DEBUG_NODE(root_item);
        BOOST_SPIRIT_DEBUG_NODE(raw_root_item);

        BOOST_SPIRIT_DEBUG_NODE(compound);
        BOOST_SPIRIT_DEBUG_NODE(basic);

        BOOST_SPIRIT_DEBUG_NODE(flatten);
        BOOST_SPIRIT_DEBUG_NODE(filter);
        BOOST_SPIRIT_DEBUG_NODE(truncate);
        BOOST_SPIRIT_DEBUG_NODE(property);
        BOOST_SPIRIT_DEBUG_NODE(index_or_range);
        BOOST_SPIRIT_DEBUG_NODE(any_root);
        BOOST_SPIRIT_DEBUG_NODE(key);
        BOOST_SPIRIT_DEBUG_NODE(quoted_string);

        on_error<fail>(root, throw_(construct<SyntaxError>(_1, _2, _3, _4)));
    }

    qi::rule<Iterator, Selectors(), ascii::space_type> root;
    qi::rule<Iterator, RootSelector(), ascii::space_type> root_item;
    qi::rule<Iterator, std::vector<SelectorNode>(), ascii::space_type>
        raw_root_item;

    qi::rule<Iterator, SelectorNode(), ascii::space_type> compound;
    qi::rule<Iterator, SelectorNode(), ascii::space_type> basic;
    qi::rule<Iterator, SelectorNode(), ascii::space_type> index_or_range;

    qi::rule<Iterator, FlattenSelector(), ascii::space_type> flatten;
    qi::rule<Iterator, FilterSelector(), ascii::space_type> filter;
    qi::rule<Iterator, TruncateSelector(), ascii::space_type> truncate;
    qi::rule<Iterator, PropertySelector(), ascii::space_type> property;
    qi::rule<Iterator, AnyRootSelector()> any_root;
    qi::rule<Iterator, KeySelector()> key;

    qi::rule<Iterator, std::string()> quoted_string;
};

template <typename Iterator>
Selectors parse_selectors(Iterator first, Iterator last) {
    Selectors selectors;
    if (!qi::phrase_parse(first, last, selectors_grammar<Iterator>(),
                          ascii::space, selectors)) {
        throw FailedToParseSelectorException("parser failed");
    }

    // fail if we did not get a full match
    if (first != last) {
        throw FailedToParseSelectorException("didn't consume all input");
    }

    return selectors;
}

/**
 * Parse a string into a selector or throw an exception.
 *
 * Throws either FailedToParseSelectorException or SyntaxError.
 */
Selectors parse_selectors(const std::string& s) {
    return parse_selectors(s.begin(), s.end());
}

} // namespace selectors

#endif
