#ifndef JSON_QUERY_JSON_PARSER_HPP
#define JSON_QUERY_JSON_PARSER_HPP

#ifdef TRACE
#define BOOST_SPIRIT_DEBUG
#endif

#include <boost/phoenix.hpp>
#include <boost/spirit/home/qi/directive/lexeme.hpp>
#include <boost/spirit/home/qi/nonterminal/debug_handler.hpp>
#include <boost/spirit/home/support/iterators/line_pos_iterator.hpp>
#include <boost/spirit/home/support/iterators/multi_pass.hpp>
#include <boost/spirit/include/qi.hpp>
#include <iterator>
#include <memory>
#include <stdexcept>

#include "../errors.hpp"
#include "types.hpp"

namespace json {

namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

class FailedToParseJsonException : public std::exception {
    const char* reason;

public:
    FailedToParseJsonException(const char* reason) : reason(reason) {}

    virtual const char* what() const noexcept override { return reason; }
};

class InnerSyntaxError : public std::exception {
    typedef boost::spirit::line_pos_iterator<std::string::const_iterator>
        Iterator;

public:
    Iterator first;
    Iterator last;
    Iterator error_pos;
    boost::spirit::info info;

    InnerSyntaxError(Iterator first, Iterator last, Iterator error_pos,
                     const boost::spirit::info& info)
        : first(first), last(last), error_pos(error_pos), info(info) {}

    virtual const char* what() const noexcept override {
        return "Syntax error";
    }
};

class SyntaxError : public std::exception {
    typedef boost::spirit::line_pos_iterator<std::string::const_iterator>
        Iterator;

    std::size_t line_num;
    // 0 indexed column
    std::size_t col_num;
    std::string line;
    std::string expected;
    std::string what_;

public:
    SyntaxError(Iterator begin, Iterator current, Iterator end,
                const std::string& what): expected(what) {
        line_num = spirit::get_line(current);
        Iterator current_line_start = spirit::get_line_start(begin, current);

        // 0 indexed column
        col_num = spirit::get_column(current_line_start, current) - 1;

        boost::iterator_range<Iterator> line_range =
            spirit::get_current_line(current_line_start, current, end);
        line = std::string(line_range.begin(), line_range.end());

        what_ = "Expected " + expected + " but got \"" + line[col_num] + "\"";
    }

    // to make this a proper std::exception
    // but not used by me (except maybe in tests)
    virtual const char* what() const noexcept override { return what_.c_str(); }

    template <typename Out> void pretty_print(Out& o) const {
        o << "Error in json (line " << line_num << ":" << col_num << ") expected " + expected + "\n";
    }
};

// Grammar created using https://tools.ietf.org/html/rfc8259 and
// https://www.json.org/
// TODO support unicode
template <typename Iterator>
struct json_grammar : qi::grammar<Iterator, JsonNode(), ascii::space_type> {
    json_grammar() : json_grammar::base_type(root) {
        using boost::phoenix::bind;
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
        using qi::char_;
        using qi::digit;
        using qi::fail;
        using qi::int_;
        using qi::lexeme;
        using qi::lit;
        using qi::on_error;
        using qi::rule;
        using qi::uint_;

        root = value[_val = construct<JsonNode>(_1)];
        value = (literal | object | array | number |
                 string)[_val = construct<JsonNode>(_1)];

        literal = lit("false")[_val = val(JsonLiteral(JSON_FALSE))] |
                  lit("true")[_val = val(JsonLiteral(JSON_TRUE))] |
                  lit("null")[_val = val(JsonLiteral(JSON_NULL))];

        object = ('{' > -(member % ',') >
                  '}')[if_(_1)[_val = construct<JsonObject>(*_1)]
                           .else_[_val = construct<JsonObject>()]];
        member =
            (string_inner > ':' >
             value)[_val = construct<std::pair<std::string, JsonNode>>(_1, _2)];

        array = ('[' > -(value % ',') >
                 ']')[if_(_1)[_val = construct<JsonArray>(*_1)]
                          .else_[_val = construct<JsonArray>()]];

        number = qi::as_string[lexeme[-char_('-') >> +digit >> -frac >> -exp]]
                              [_val = construct<JsonNumber>(_1)];
        frac = char_('.') >> +digit;
        exp = (char_('e') | char_('E')) >> -char_('-') >> +digit;

        string = string_inner[_val = construct<JsonString>(_1)];

        string_inner = '"' > lexeme[+(unescaped | escaped)[_val += _1]] > '"';
        unescaped = char_ - '"' - '\\' - ascii::cntrl;
        escaped =
            char_('\\') > (char_('\\') | char_('"') | char_('n') | char_('b') |
                           char_('f') | char_('r') | char_('t') |
                           (char_('u') >> qi::repeat(1, 4)[ascii::xdigit]));

        on_error<fail>(root,
                       throw_(construct<InnerSyntaxError>(_1, _2, _3, _4)));
    }

    qi::rule<Iterator, JsonNode(), ascii::space_type> root;
    qi::rule<Iterator, JsonNode(), ascii::space_type> value;
    qi::rule<Iterator, JsonNode()> literal;
    qi::rule<Iterator, JsonNode(), ascii::space_type> object;
    qi::rule<Iterator, std::pair<std::string, JsonNode>(), ascii::space_type>
        member;
    qi::rule<Iterator, JsonNode(), ascii::space_type> array;
    qi::rule<Iterator, JsonNode()> number;
    qi::rule<Iterator, std::string()> frac;
    qi::rule<Iterator, std::string()> exp;
    qi::rule<Iterator, JsonNode()> string;

    qi::rule<Iterator, std::string()> string_inner;
    qi::rule<Iterator, std::string()> unescaped;
    qi::rule<Iterator, std::string()> escaped;
};

/**
 * Parses a string into a json object or throw an exception.
 *
 * Throws either FailedToParseJsonException or SyntaxError.
 *
 * NOTE: There is only a string version provided. For one this allows for
 * better error messages because we can display the line where the error
 * occured. This is not possible using e.g. std::ifstream. And also I couldn't
 * solve the error when trying to use std::ifstream with
 * boost::spirit::line_pos_iterator<boost::spirit::istream_iterator> as the
 * Iterator type. It would just crash with an std::__ios_failure exception with
 * the message "basic_ios::clear: iostream error".
 */
JsonNode parse_json(const std::string& s) {
    typedef boost::spirit::line_pos_iterator<std::string::const_iterator>
        Iterator;
    Iterator begin(s.cbegin());
    Iterator end(s.cend());

    JsonNode json;
    try {
        bool ok = qi::phrase_parse(begin, end, json_grammar<Iterator>(),
                                   ascii::space, json);

        if (!ok || begin != end) {
            throw FailedToParseJsonException("parser failed");
        }
    } catch (InnerSyntaxError& e) {
        std::string expected;
        std::stringstream ss;
        ss << e.info;
        expected = ss.str();

        throw SyntaxError(Iterator(s.cbegin()), e.error_pos, end, expected);
    }

    return json;
}

} // namespace json

#endif
