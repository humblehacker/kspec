%top {
  #define YYLTYPE yy::location
  #define YY_EXTRA_TYPE Scanner*
}

%{
  #include <cassert>
  #include <sstream>
  #include <fstream>
  #include <iomanip>
  
  #include "scanner.h"
  #include "location.hh"

  #define YY_USER_ACTION  { useraction(ctx, yylloc_param); }
  #define IGNORE(THING)   { }

  #define YY_DECL \
    int Scanner::lex(yy::Parser::semantic_type *yylval_param, \
                     yy::Parser::location_type *yylloc_param, \
                     parser_context &ctx)

  typedef yy::Parser::token token;

  #define YY_FATAL_ERROR(msg) \
     { \
       Scanner * scnr = yyget_extra(yyscanner); \
       assert(scnr); \
       Scanner::fatal_error error(*scnr->location()); \
       throw error << msg;\
     }

  enum yytokentype { IDENT = 258 };
%}

%option 8bit
%option backup
%option bison-bridge
%option bison-locations
%option debug
%option ecs
/* %option nodefault */
%option nounput
%option noyywrap
%option header-file="scanner.lex.h"
%option perf-report
%option reentrant
/* %option verbose */

%%

[\-a-zA-Z0-9_]+   { return IDENT; }

%%


