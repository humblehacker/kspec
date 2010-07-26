#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <iterator>
#include <cassert>
#include <string>
#include "boost/filesystem.hpp"
#include "Options.h"
#include "Scanner.h"
#include "Parser.h"
#include "lua_helpers.h"
#include "lua_keyboard_visitor.h"

using namespace std;
namespace fs = boost::filesystem;

hh::Keyboard::Ptr parse(const Options &options);
void dump(hh::Keyboard &kb);
void build_lua_environment(const hh::Keyboard &kb, lua_State *L);
void generate(const fs::path &filename, lua_State *L);
void go_interactive(lua_State *L);

int
main (int argc, char *argv[])
{
  try
  {
    hh::Keyboard::Ptr kb;
    Options options(argc, argv);
    kb = parse(options);
    assert(kb);
//  dump(*kb);

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    build_lua_environment(*kb, L);

    fs::path output_dir("generated");
    fs::create_directories(output_dir);
    generate(output_dir / "matrix.h", L);
    generate(output_dir / "binding.h", L);

    if (options.interactive())
      go_interactive(L);

    lua_close(L);
  }
  catch ( const option_error &e )
  {
    cerr << "Option Error: " << e.what() << endl;
    Options::usage();
    return 1;
  }
  catch(exception &e)
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
  wcout << "  Matrix: #cols: "  << kb.matrix().front()->size() << endl;
  wcout << "  RowPins: " << kb.row_pins() << endl;
  wcout << "  ColPins: " << kb.col_pins() << endl;
  wcout << "Keymaps #: " << kb.maps().size() << endl;
  for (hh::KeyMaps::const_iterator i = kb.maps().begin(); i != kb.maps().end(); ++i)
  {
    wcout << "  Keymap: " << i->first << endl;
    wcout << "    base: " << i->second->base() << endl;
    wcout << "    default: " << i->second->default_map() << endl;
    wcout << "    #Keys: " << i->second->keys().size() << endl;
  }
}

void
go_interactive(lua_State *L)
{
  int error;
  std::string line;
  cout << "> ";
  while (getline(cin, line))
  {
    error = luaL_loadbuffer(L, line.c_str(), line.length(), "line") || lua_pcall(L, 0, 0, 0);
    if (error)
    {
      cerr << lua_tostring(L, -1) << endl;
      lua_pop(L, -1);
    }
    cout << "> ";
  }
}

hh::Keyboard::Ptr
parse(const Options &options)
{
  wstring filename(options.filename().length(), L' '); // Make room for characters

  // Copy string to wstring.
  copy(options.filename().begin(), options.filename().end(), filename.begin());
  Scanner *scanner = new Scanner(filename.c_str());
  Parser  *parser  = new Parser(scanner);
  parser->Parse();
  cerr << parser->errors->count << " errors detected" << endl;
  return parser->kb;
}


void
generate(const fs::path &output_filename, lua_State *L)
{
  // build the template filename from the output filename, and open it
  fs::path template_filename("templates");
  template_filename /= output_filename.stem() + ".elu" + output_filename.extension();
  ifstream template_file(template_filename.string().c_str());

  // Parse the template file, and build the Lua generator script
  enum States
  {
    ROOT,
    EMBEDDED_STATEMENT,
    EMBEDDED_SUBSTITUTION
  } state = ROOT;

  stringstream out;
  out << "outfile = io.open('" << output_filename << "', 'w')" << endl;
  size_t prev_pos, found_pos;
  string line;
  bool out_newline;
  while (getline(template_file, line))
  {
    out_newline = true;
    prev_pos = 0;
    while (prev_pos < line.length() && prev_pos != string::npos)
    {
      switch (state)
      {
      case ROOT:
        found_pos = line.find("<%", prev_pos);
        if (found_pos != 0)
        {
          out << "outfile:write('"
              << line.substr(prev_pos, found_pos==string::npos?found_pos:found_pos-prev_pos)
              << "'); ";
        }
        if (found_pos != string::npos)
        {
          if (found_pos == 0)
            out_newline = false;
          found_pos += 2;
          if (line[found_pos] == '=')
          {
            state = EMBEDDED_SUBSTITUTION;
            ++found_pos;
          }
          else
            state = EMBEDDED_STATEMENT;
        }
        break;
      case EMBEDDED_STATEMENT:
      case EMBEDDED_SUBSTITUTION:
        found_pos = line.find("%>", prev_pos);
        if (state == EMBEDDED_SUBSTITUTION)
          out << "outfile:write(";
        out << line.substr(prev_pos, found_pos==string::npos?found_pos:found_pos-prev_pos);
        if (state == EMBEDDED_SUBSTITUTION)
          out << "); ";
        if (found_pos != string::npos)
        {
          state = ROOT;
          found_pos += 2;
        }
        break;
      }
      prev_pos = found_pos;
    }
    if (state == ROOT && out_newline)
      out << "outfile:write('\\n')";
    out << endl;
  }
  out << "outfile:close(); " << endl;;

  cout << out.str();

  // Compile and execute the Lua script
  int error = luaL_loadstring(L, out.str().c_str());
  if (error)
  {
    cerr << lua_tostring(L, -1) << endl;
    lua_pop(L, 1);
  }

  lua_pcall(L, 0, 0, 0);
}

void
build_lua_environment(const hh::Keyboard &kb, lua_State *L)
{
  int index;

  lua_newtable(L);
  hh::LuaKeyboardVisitor v(L);
  kb.accept(v);
  lua_setglobal(L, "kb");
}


