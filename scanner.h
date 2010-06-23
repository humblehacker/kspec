#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "location_exception.h"
#include "astree.h"
#include "hashtable.h"
#include "parser.hh"
#include "position.hh"
#include "shared_ptr.h"

typedef HashTable<std::string> StringTable;

class Scanner
{
public:
  Scanner(FILE *fp);
  Scanner(const std::string &input_filename, bool internal=false);
  ~Scanner();
  void initialize(const std::string &input_filename);
  typedef shared_ptr<Scanner> Ptr;

  int lex(yy::Parser::semantic_type *lval,
          yy::Parser::location_type *loc,
          parser_context &ctx);

  const char *text()   const;
  size_t length() const;
  std::string str() const;

  FILE    *infile() const;
  void     set_infile(FILE *in_str);

  FILE    *outfile() const;
  void     set_outfile(FILE *out_str);

  int      lineno() const;
  void     set_lineno(int line_number);

  ASTree::Ptr *lval();
  void     set_lval(ASTree::Ptr *yylval_p);

  const yy::location *location() const;
  void  set_location(yy::location *lloc);

  int  debug() const;
  void set_debug(int flag);

  const StringTable & strings() const { return stringtable; }

  class fatal_error : public location_exception
  {
  public:
    fatal_error(const yy::location & loc) : location_exception(loc) {}
    template <typename T>
    const fatal_error &operator<<(T &rhs)
      { location_exception::operator<<(rhs); return *this; }
  };
#if defined(__SUNPRO_CC)
#pragma error_messages(on, hidef)
#endif

private:
  int new_token(Token::Type code);
  void set_echo(bool echo_) { echo = echo_; }
  void useraction(parser_context &,
                  yy::Parser::location_type *);
  void badchar (unsigned char bad);

private:
  std::string input_filename;
  std::string token_filename;
  yyscan_t yyscanner;   // flex's reentrant scanner structure
  bool echo;
  bool internal;

  StringTable stringtable;
};

#endif // __SCANNER_H__
