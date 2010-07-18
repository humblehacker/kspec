#include <iostream>
#include <fstream>
#include <cstdio>
#include <iterator>
#include <cassert>

#include "Options.h"
#include "Scanner.h"
#include "Parser.h"

using namespace std;

hh::Keyboard *parse(const Options &options);

int
main (int argc, char *argv[])
{
  try
  {
    hh::Keyboard *kb = NULL;
    Options options(argc, argv);
    cout << "input filename: " << options.filename() << endl;
    kb = parse(options);
    assert(kb);
    wcout << "Keyboard:" << kb->ident() << endl;
    wcout << "Matrix: #rows: "  << kb->matrix().size() << endl;
    wcout << "Matrix: #cols: "  << kb->matrix().front().size() << endl;
    wcout << "RowPins: " << kb->rpins() << endl;
    wcout << "ColPins: " << kb->cpins() << endl;
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
  catch(...)
  {
    cerr << "Unhandled exception" << endl;
    return 1;
  }
  return 0;
}

hh::Keyboard *
parse(const Options &options)
{
  std::wstring filename(options.filename().length(), L' '); // Make room for characters

  // Copy string to wstring.
  std::copy(options.filename().begin(), options.filename().end(), filename.begin());
  Scanner *scanner = new Scanner(filename.c_str());
  Parser  *parser  = new Parser(scanner);
  parser->Parse();
  cout << parser->errors->count << " errors detected" << endl;
  return parser->kb;
}






















