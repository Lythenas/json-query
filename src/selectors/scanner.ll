%{ /* -*- C++ -*- */
#include "selector_parser.hpp"
#include "selector_type.hpp"
#include "driver.hpp"
%}

%option noyywrap nounput noinput batch debug

str   ".*"
int   [0-9]+
blank [ \t\r]

%%
%{
%}

{blank}+ ;

"."     return yy::selector_parser::make_DOT();
{str}   return yy::selector_parser::make_KEY(yytext);

<<EOF>> return yy::selector_parser::make_YYEOF();


%%

void SelectorDriver::scan_begin() {
  // yy_flex_debug = false; // trace_scanning
  yy_scan_buffer(input, size);
}

void SelectorDriver::scan_end() {
}
