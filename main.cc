#include <iostream>
#include <fstream>
#include <cstdio>
#include <iterator>
#include <boost/foreach.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#define foreach BOOST_FOREACH

#include "Options.h"

using namespace std;
using namespace boost::spirit;

class Keyboard
{
  std::string _ident;
public:
  void set_ident(const std::string &ident) { _ident = ident; }
  const std::string &ident() const { return _ident; }
};

void parse(const Options &options, Keyboard &kb);

int
main (int argc, char *argv[])
{
  try
  {
    Options options(argc, argv);
    cout << "input filename: " << options.filename() << endl;
    Keyboard kb;
    parse(options, kb);
    cout << "Keyboard:" << kb.ident() << endl;
  }
  catch ( const option_error &e )
  {
    cerr << "Option Error: " << e.what() << endl;
    Options::usage();
    return 1;
  }
  catch(std::exception &e)
  {
    cerr << e.what() << endl;
    Options::usage();
    return 1;
  }
  return 0;
}

extern "C" {
  void yyerror(char *s)
  {
    fprintf(stderr, "error: %s\n", s);
  }
}

namespace client
{
  template <typename Iterator>
  bool parse(Iterator first, Iterator last, Keyboard &kb)
  {
    using ascii::space;
    using ascii::char_;
    using ascii::alpha;
    using ascii::digit;
    using qi::lit;
    using qi::_1;
    using qi::lexeme;
    using qi::phrase_parse;
    using boost::phoenix::ref;

    string ident, loc;

    bool r = phrase_parse(first, last,
      // begin grammar
      (
        lit("Keyboard:") >> +(char_)[ref(ident) += _1] >>
        lit("Matrix:") >>
        lit("Row:") >> ("--" | (digit > alpha))[ref(loc) += _1.first][ref(loc) += _1.second]
      ),
      space);

    cout << "identifier: " << ident << endl;

    if (!r || first != last)
      return false;

    kb.set_ident(ident);
    return r;
  }
}

void
parse(const Options &options, Keyboard &kb)
{
  ifstream input(options.filename().c_str());
  std::string str;
  while (getline(input, str))
  {
    if (!client::parse(str.begin(), str.end(), kb))
      return;
  }
}





