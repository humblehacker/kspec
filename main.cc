#include <iostream>
#include <fstream>
#include <cstdio>
#include <iterator>

#include "Options.h"
#include "Scanner.h"
#include "Parser.h"

using namespace std;

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

void
parse(const Options &options, Keyboard &kb)
{
  std::wstring filename(options.filename().length(), L' '); // Make room for characters

  // Copy string to wstring.
  std::copy(options.filename().begin(), options.filename().end(), filename.begin());
  Scanner *scanner = new Scanner(filename.c_str());
  Parser  *parser  = new Parser(scanner);
  parser->Parse();
  cout << parser->errors->count << " errors detected" << endl;
}





