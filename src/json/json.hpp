#include "parser.hpp"


// GRAMMAR
// https://tools.ietf.org/html/rfc8259
// https://www.json.org/
//
// whitespace is allowed and ignored everywhere except in literals
// literals are marked as strings (in "") also number is a literal
//
// JSON-text = value
// begin-array = "["
// begin-object = "{"
// end-array = "]"
// end-object = "}"
// name-separator = ":"
// value-separator = ","
// quotation-mark = """
//
// value = "false" | "null" | "true" | object | array | number | string
//
// object = begin-object [ member *( value-separator member ) ] end-object
// member = string name-separator value
//
// array = begin-array [ value *( value-separator value ) ] end-array
//
// number = [ minus ] int [ frac ] [ exp ]
// decimal-point = "."
// digit1-9 = "1"-"9"
// e = "e" | "E"
// exp = e [ minus | plus ] +DIGIT
// frac = "." +DIGIT
// int = zero | ( digit1-9 *DIGIT )
// minus = "-"
// plus = "+"
// zero = "0"
//
// string = quotation-mark *char quotation-mark
// char = unescaped | escape ( """ | "\" | "/" | "b" | "f" | "n" | "r" | "t" | ( "u" 4HEX ) )
// escape = "\"
// unescaped = %x20-21 | %x23-5B | %x5D-10FFFF
//  (anything except " or \ or control characters)
