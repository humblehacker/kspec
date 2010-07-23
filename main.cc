#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <iterator>
#include <cassert>
#include <string>

#include "Options.h"
#include "Scanner.h"
#include "Parser.h"

extern "C"
{
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
}

using namespace std;

hh::KeyboardPtr parse(const Options &options);
void dump(hh::Keyboard &kb);
void build_lua_environment(const hh::Keyboard &kb, lua_State *L);
void generate(const string &filename, lua_State *L);

int
main (int argc, char *argv[])
{
  try
  {
    hh::KeyboardPtr kb;
    Options options(argc, argv);
    kb = parse(options);
    assert(kb);
//  dump(*kb);

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    build_lua_environment(*kb, L);
    generate("matrix.h", L);

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
  wcout << "  RowPins: " << kb.rpins() << endl;
  wcout << "  ColPins: " << kb.cpins() << endl;
  wcout << "Keymaps #: " << kb.maps().size() << endl;
  for (hh::KeyMaps::const_iterator i = kb.maps().begin(); i != kb.maps().end(); ++i)
  {
    wcout << "  Keymap: " << i->first << endl;
    wcout << "    base: " << i->second->base() << endl;
    wcout << "    default: " << i->second->default_map() << endl;
    wcout << "    #Keys: " << i->second->keys().size() << endl;
  }
}

hh::KeyboardPtr
parse(const Options &options)
{
  wstring filename(options.filename().length(), L' '); // Make room for characters

  // Copy string to wstring.
  copy(options.filename().begin(), options.filename().end(), filename.begin());
  Scanner *scanner = new Scanner(filename.c_str());
  Parser  *parser  = new Parser(scanner);
  parser->Parse();
  cout << parser->errors->count << " errors detected" << endl;
  return parser->kb;
}

void
lprint(const string &what, ostream &where)
{
  where << "io.write(\"" << slash(what) << "\"); ";
}

void
generate(const string &filename, lua_State *L)
{
  // Open the file
  ifstream template_file("templates/matrix.erb.h");
  stringstream out;
  enum States { ROOT, EMBEDDED_STATEMENT, EMBEDDED_SUBSTITUTION };
  States state = ROOT;

  // for each line in file
  size_t pos, end_pos;
  string line;
  while (getline(template_file, line))
  {
    pos = 0;
    while (pos < line.length() && pos != string::npos)
    {
      switch (state)
      {
      case ROOT:
        end_pos = line.find("<%", pos);
        if (end_pos != 0)
          lprint(line.substr(pos, end_pos==string::npos?end_pos:end_pos-pos), out);
        if (end_pos != string::npos)
        {
          end_pos += 2;
          if (line[end_pos] == '=')
          {
            state = EMBEDDED_SUBSTITUTION;
            ++end_pos;
          }
          else
            state = EMBEDDED_STATEMENT;
        }
        break;
      case EMBEDDED_STATEMENT:
      case EMBEDDED_SUBSTITUTION:
        end_pos = line.find("%>", pos);
        if (state == EMBEDDED_SUBSTITUTION)
          out << "io.write(";
        out << line.substr(pos, end_pos==string::npos?end_pos:end_pos-pos) << " ";
        if (state == EMBEDDED_SUBSTITUTION)
          out << "); ";
        if (end_pos != string::npos)
          state = ROOT;
        end_pos += 2;
        break;
      }
      pos = end_pos;
    }
    if (state == ROOT)
      out << "io.write('\\n')";
    out << endl;
  }

  cout << out.str() << endl;
  int error = luaL_loadstring(L, out.str().c_str());
  if (error)
  {
    cout << lua_tostring(L, -1) << endl;
    lua_pop(L, 1);
  }

  lua_pcall(L, 0, 0, 0);
}

void build_lua_environment(const hh::Keyboard &kb, lua_State *L)
{
  lua_newtable(L);
  lua_pushstring(L, "row_count");
  lua_pushnumber(L,kb.matrix().size());
  lua_settable(L, -3);
  lua_pushstring(L, "col_count");
  lua_pushnumber(L,kb.matrix().front()->size());
  lua_settable(L, -3);
  lua_setglobal(L, "matrix");

  lua_pushboolean(L, kb.block_ghost_keys());
  lua_setglobal(L, "block_ghost_keys");
}

