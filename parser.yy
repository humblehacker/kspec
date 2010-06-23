%language "C++"
%defines
%locations
%skeleton "./lalr1.cc"
%require "2.4"
%verbose

%define parser_class_name "Parser"

// inserted towards top of parser.hh
%code requires {
  #include "astree.h"
  #define YYSTYPE ASTree::Ptr
  #include "parser_context.h"
  typedef void* yyscan_t;
  class Scanner;
  extern unsigned error_count;
  extern std::string last_error;
  extern std::ostream *err;
}

// inserted towards bottom of parser.hh
%code provides {
  namespace yy {
    void error (const yy::location &loc, const std::string &msg);
  }
}

// inserted into parser.cc before #include "parser.hh"
%code top {
}

// inserted in parser.cc after #include "parser.hh"
%code {
  #include "scanner.h"
  #ifdef yylex
  #undef yylex
  #endif
  #define yylex ctx.scanner->lex
  unsigned error_count = 0;
  std::string last_error;
  std::ostream *err = &std::cerr;
}

%parse-param { parser_context &ctx  }
%lex-param   { parser_context &ctx  }

%{

%}

%%

keyboard:

/*
matrix:

matrix_row:

port:

keymap:

key:

location:

labels:

map:

mode:

macro:

usage:

usage_page:

modifiers:

label:

modifier_list:

modifier:

coded_char:

string:
*/

%%

namespace yy {
  void
  error(location const &loc, const std::string &msg) {
    assert(err);
    ++error_count;
    std::stringstream str;
    str << loc << ": " << msg;
    last_error = str.str();
    (*err) << str.str() << std::endl;
  }
  void
  Parser::error(location const &loc, const std::string &msg) {
    ::yy::error(loc, msg);
  }
}

