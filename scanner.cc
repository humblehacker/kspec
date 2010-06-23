#include <iostream>
#include <fstream>
#include <iomanip>

#include "scanner.h"
#include "scanner.lex.h"

using yy::location;
using std::string;

Scanner::
Scanner(FILE *in)
{
  initialize(std::string("temp"));
  assert(in);
  set_infile(in);
}

Scanner::
Scanner(const std::string &input_filename_, bool internal) : echo(false),
  internal(internal)
{
  initialize(input_filename_);
  FILE *in = fopen(input_filename.c_str(), "r");
  if (in == NULL)
    throw fatal_error(*location()) << "yyin undefined";
  set_infile(in);
}

Scanner::
~Scanner()
{
  FILE *in = infile();
  if (in) fclose(in);
  yylex_destroy(yyscanner);
}

void
Scanner::
initialize(const std::string &input_filename_)
{
  yylex_init(&yyscanner);
  yyset_extra(this, yyscanner);
  input_filename = input_filename_;
  token_filename = input_filename_;
//token_filename.setExtension("tok");
  // overwrite previous token file
//std::ofstream token_file(token_filename.c_str());
  echo = false;
}

const char *
Scanner::
text() const
{
  return yyget_text(yyscanner);
}

size_t
Scanner::
length() const
{
  return yyget_leng(yyscanner);
}

std::string
Scanner::
str() const
{
  return string(yyget_text(yyscanner));
}


FILE *
Scanner::
infile() const
{
  return yyget_in(yyscanner);
}

FILE *
Scanner::
outfile() const
{
  return yyget_out(yyscanner);
}

int
Scanner::
lineno() const
{
  return yyget_lineno(yyscanner);
}

int
Scanner::
debug() const
{
  return yyget_debug(yyscanner);
}


void
Scanner::
set_infile(FILE *in_str)
{
  yyset_in(in_str, yyscanner);
}

void
Scanner::
set_outfile(FILE *out_str)
{
  yyset_out(out_str, yyscanner);
}

void
Scanner::
set_lineno(int line_number)
{
  yyset_lineno(line_number, yyscanner);
}

ASTree::Ptr *
Scanner::
lval()
{
  return yyget_lval(yyscanner);
}

void
Scanner::
set_lval(ASTree::Ptr *yylval_p)
{
  yyset_lval(yylval_p, yyscanner);
}

const yy::location *
Scanner::
location() const
{
  return yyget_lloc(yyscanner);
}

void
Scanner::
set_location(yy::location *lloc)
{
  yyset_lloc(lloc, yyscanner);
}

void
Scanner::
set_debug(int flag)
{
  yyset_debug(flag, yyscanner);
}

void
dump_token(std::ostream &os, const ASTree &node)
{
  using namespace std;
  static const int mult = 1000; // serial number multiplier
  const yy::location &loc = node.location();
  double serial = (loc.begin.line * mult + loc.begin.column) / mult;

  // 1st column: (double) serial_nr / 1000.0 in %8.3f format.
  os << "  " << right << fixed << setw(8) << setprecision(3) << serial;
  // 2nd column: integer token_code.
  os << "  " << setw(3) << node.code();
  // 3rd column: symbolic name of the token code.
  os << "  " << setw(10) << left;
  const char *tok_name = yy::Parser::token_name(node.code());
  if ( strcmp(tok_name, "$undefined") == 0)
    os << node.code();
  else
    os << tok_name;
  // 4th column: lexical information associated with the token printed
  //             btw parentheses exactly as stored in the stringtable.
  os << " (" << node.name() << ")" << endl;
}

int
Scanner::
new_token (Token::Type code)
{
  using namespace std;
  ofstream token_file(token_filename.c_str(), ios_base::out | ios_base::app);

  ASTree::Ptr ast = ASTree::construct(code, *location(),
                                      stringtable.intern(text())->key);
  ast->token().internal = internal;
  dump_token(token_file, *ast);
  assert(lval());
  *lval() = ast;

  return static_cast<int>(code);
}

void
Scanner::
useraction(parser_context &ctx, yy::Parser::location_type *loc)
{
  if (echo)
  {
    if (loc->end.column == 0) printf (";%5d: ", loc->end.line);
    printf ("%s", text());
  }
  loc->begin.filename = loc->end.filename = &ctx.filename;
  loc->step();
  loc->columns(length());
}

void
Scanner::
badchar (unsigned char bad)
{
  using namespace std;
  const yy::location *loc = location();
  assert(loc);
  stringstream msg;
  msg << " bad input character (";
  if (!isgraph(static_cast<int>(bad))) msg << setfill('0') << setw(3);
  msg << bad << ")";
  yy::error(*loc, msg.str());
}

