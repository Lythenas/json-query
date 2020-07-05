#ifndef JSON_QUERY_JSON_PARSER_HPP
#define JSON_QUERY_JSON_PARSER_HPP

#include <boost/spirit/home/qi/directive/lexeme.hpp>
#ifndef NDEBUG
#undef BOOST_SPIRIT_DEBUG
#endif

#include <boost/phoenix.hpp>
#include <boost/spirit/home/qi/nonterminal/debug_handler.hpp>
#include <boost/spirit/include/qi.hpp>
#include <iterator>
#include <memory>
#include <stdexcept>

#include "types.hpp"

namespace json {

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    template <typename Iterator>
    struct json_grammar : qi::grammar<Iterator, Json(), ascii::space_type> {
        json_grammar() : json_grammar::base_type(root) {
            using boost::phoenix::bind;
            using boost::phoenix::construct;
            using boost::phoenix::new_;
            using boost::phoenix::val;
            using qi::_1;
            using qi::_2;
            using qi::_3;
            using qi::_val;
            using qi::char_;
            using qi::digit;
            using qi::int_;
            using qi::lexeme;
            using qi::lit;
            using qi::rule;
            using qi::uint_;

            root = value[_val = construct<Json>(_1)];
            value = (literal | object | array | number |
                     string)[_val = construct<JsonNode>(_1)];

            literal = lit("false")[_val = val(JsonLiteral(JSON_FALSE))] |
                      lit("true")[_val = val(JsonLiteral(JSON_TRUE))] |
                      lit("null")[_val = val(JsonLiteral(JSON_NULL))];

            object =
                ('{' >> -(member % ',') >> '}')[_val = construct<JsonObject>(_1)];
            member =
                (string_inner >> ':' >>
                 value)[_val = construct<std::pair<std::string, JsonNode>>(_1, _2)];

            array = ('[' >> -(value % ',') >> ']')[_val = construct<JsonArray>(_1)];

            number = qi::as_string[lexeme[-char_('-') >> +digit >> -frac >> -exp]]
                                  [_val = construct<JsonNumber>(_1)];
            frac = char_('.') >> +digit;
            exp = (char_('e') | char_('E')) >> -char_('-') >> +digit;

            string = string_inner[_val = construct<JsonString>(_1)];

            string_inner = '"' >> lexeme[+(unescaped | escaped)[_val += _1]] >> '"';
            unescaped = char_ - '"' - '\\' - ascii::cntrl;
            escaped =
                char_('\\') >> (char_('\\') | char_('"') | char_('n') | char_('b') |
                                char_('f') | char_('r') | char_('t') |
                                (char_('u') >> qi::repeat(1, 4)[ascii::xdigit]));
        }

        qi::rule<Iterator, Json(), ascii::space_type> root;
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

    template <typename Iterator>
    Json parse_json(Iterator first, Iterator last) {
        Json json;
        if (!qi::phrase_parse(first, last, json_grammar<Iterator>(), ascii::space,
                              json)) {
            throw std::runtime_error("json parser failed");
        }

        if (first != last) {
            throw std::runtime_error("json didn't consume all input");
        }

        return json;
    }

} // namespace json

#endif
