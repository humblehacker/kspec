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
void dump(hh::Keyboard &kb);

int
main (int argc, char *argv[])
{
  try
  {
    hh::Keyboard *kb = NULL;
    Options options(argc, argv);
    kb = parse(options);
    assert(kb);
    dump(*kb);
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

void dump(hh::Keyboard &kb)
{
  wcout << "Keyboard: " << kb.ident() << endl;
  wcout << "  Matrix: #rows: "  << kb.matrix().size() << endl;
  wcout << "  Matrix: #cols: "  << kb.matrix().front().size() << endl;
  wcout << "  RowPins: " << kb.rpins() << endl;
  wcout << "  ColPins: " << kb.cpins() << endl;
  wcout << "Keymaps #: " << kb.maps().size() << endl;
  for (hh::KeyMaps::const_iterator i = kb.maps().begin(); i != kb.maps().end(); ++i)
  {
    wcout << "  Keymap: " << i->first << endl;
    wcout << "    base: " << i->second.base() << endl;
    wcout << "    default: " << i->second.default_map() << endl;
    wcout << "    #Keys: " << i->second.keys().size() << endl;
  }
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






















