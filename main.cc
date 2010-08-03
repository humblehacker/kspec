#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <iterator>
#include <cassert>
#include <string>
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include "Scanner.h"
#include "Parser.h"
#include "lua_helpers.h"
#include "lua_keyboard_visitor.h"

using namespace std;
namespace fs = boost::filesystem;
namespace po = boost::program_options;

hh::Keyboard::Ptr parse(const wstring &input_filename);
void dump(hh::Keyboard &kb);
void build_lua_environment(const hh::Keyboard &kb, lua_State *L);
void generate(const fs::path &template_filename, const fs::path &output_dir,
              lua_State *L, const po::variables_map &options);
void go_interactive(lua_State *L);
bool process_options(int argc, char *argv[], po::options_description &usage,
                     po::variables_map &options);
void generate_code(po::variables_map &options, hh::Keyboard::Ptr &kb);

int
main (int argc, char *argv[])
{
  po::options_description usage("kspec [options] input-file");
  po::variables_map options;
  try
  {
    if (!process_options(argc, argv, usage, options))
      return 0;

    wstring input_filename;
    string_to_wstring(options["input-file"].as<string>(), input_filename);

    hh::Keyboard::Ptr kb;
    kb = parse(input_filename);

    if (options.count("generate-code"))
    {
      generate_code(options, kb);
    }
    else if (options.count("pdf") || options.count("png") || options.count("svg")
             || options.count("program") || options.count("extract"))
    {
      cerr << "Option not yet implemented" << endl;
    }

  }
  catch ( const po::error &e )
  {
    cerr << "Option Error: " << e.what() << endl;
    cout << usage << endl;
    return 1;
  }
  catch(exception &e)
  {
    cerr << e.what() << endl;
    cout << usage << endl;
    return 1;
  }
  catch(const char *e)
  {
    cerr << "Unhandled exception: " << e << endl;
    return 1;
  }
  catch(...)
  {
    cerr << "Unhandled exception" << endl;
    return 1;
  }
  return 0;
}

bool
process_options(int argc, char *argv[],
                po::options_description &usage,
                po::variables_map &options)
{
  po::options_description gen_desc("General options");
  gen_desc.add_options()
    ("version,v",        "display the version number")
    ("help,h",           "produce this help text")
  ;
  po::options_description cg_desc("Code generation options");
  cg_desc.add_options()
    ("generate-code,g",  "generate code for keyboard firmware")
    ("output-dir,o",   po::value<string>()->default_value("./generated"),
     "destination for generated code files")
    ("template-dir,t", po::value<string>()->default_value("./templates"),
     "location of code-generation template files")
  ;
  po::options_description dl_desc("Layout display options");
  dl_desc.add_options()
    ("pdf", "produce keyboard images in PDF format")
    ("svg", "produce keyboard images in SVG format")
    ("png", "produce keyboard images in PNG format")
  ;
  po::options_description pg_desc("Programming options");
  pg_desc.add_options()
    ("program", "send keymaps to keyboard")
    ("extract", "read keymaps from keyboard")
  ;

  po::options_description invisible("Invisible options");
  invisible.add_options()
    ("input-file", po::value<string>(), "kspec input file")
    ("debug,d", "produce extra debugging output");

  po::positional_options_description pos_desc;
  pos_desc.add("input-file", 1);

  po::options_description all_desc("");
  all_desc.add(gen_desc).add(cg_desc).add(dl_desc).add(pg_desc).add(invisible);

  usage.add(gen_desc).add(cg_desc).add(dl_desc).add(pg_desc);

  po::command_line_parser parser(argc, argv);
  po::store(parser.options(all_desc).positional(pos_desc).run(), options);
  po::notify(options);

  if (options.count("help"))
  {
    cout << usage;
    return false;
  }

  return true;
}

void
generate_code(po::variables_map &options, hh::Keyboard::Ptr &kb)
{
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  build_lua_environment(*kb, L);

  fs::path template_dir(options["template-dir"].as<string>());
  if (!fs::exists(template_dir) || !fs::is_directory(template_dir))
    throw "invalid template-dir specified";

  int error = luaL_loadfile(L, (template_dir / "shared.lua").string().c_str());
  if (error)
  {
    cerr << lua_tostring(L, -1) << endl;
    lua_pop(L, 1);
  }
  error = lua_pcall(L, 0, 0, 0);
  if (error)
  {
    cerr << lua_tostring(L, -1) << endl;
    lua_pop(L, 1);
  }

///////////////////////////////////////////////////////////////////////////
  fs::path output_dir(options["output-dir"].as<string>());
  fs::create_directories(output_dir);

  fs::directory_iterator end_iter;
  for (fs::directory_iterator dir_itr(template_dir); dir_itr != end_iter; ++dir_itr)
  {
    try
    {
      if (fs::is_regular_file(dir_itr->status()) && boost::iends_with(dir_itr->path().stem(), ".elu"))
      {
        generate(dir_itr->path(), output_dir, L, options);
      }
    }
    catch ( const std::exception & ex )
    {
      std::cout << dir_itr->path().filename() << " " << ex.what() << std::endl;
    }
  }
///////////////////////////////////////////////////////////////////////////

  if (options.count("interactive"))
    go_interactive(L);

  lua_close(L);
}

void
dump(hh::Keyboard &kb)
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
parse(const wstring &input_filename)
{
  Scanner *scanner = new Scanner(input_filename.c_str());
  Parser  *parser  = new Parser(scanner);
  parser->Parse();
  cerr << parser->errors->count << " errors detected" << endl;
  return parser->kb;
}


void
generate(const fs::path &template_filename, const fs::path &output_dir, lua_State *L,
         const po::variables_map &options)
{
  // build the template filename from the output filename, and open it
  string stem = template_filename.stem();
  boost::replace_all(stem, ".elu", "");
  fs::path output_filename(stem + template_filename.extension());
  output_filename = output_dir / output_filename;

  cout << "generating " << output_filename << " from " << template_filename << endl;

  ifstream template_file(template_filename.string().c_str());

  // Parse the template file, and build the Lua generator script
  enum States
  {
    ROOT,
    EMBEDDED_STATEMENT,
    EMBEDDED_SUBSTITUTION
  } state = ROOT;

  stringstream out;
  out << " outfile = io.open('" << output_filename << "', 'w'); " << endl;
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
          std::string piece(line.substr(prev_pos, found_pos==string::npos?found_pos:found_pos-prev_pos));
          boost::replace_all(piece, "'", "\\'");
          out << " outfile:write('" << piece << "'); ";
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
          out << " outfile:write( ";
        out << line.substr(prev_pos, found_pos==string::npos?found_pos:found_pos-prev_pos);
        if (state == EMBEDDED_SUBSTITUTION)
          out << " ); ";
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
      out << " outfile:write('\\n'); ";
    out << endl;
  }
  out << " outfile:close(); " << endl;;

  if (options.count("debug"))
  {
    int linenum = 1;
    while (getline(out, line))
    {
      cout << setw(4) << dec << linenum++ << "| " << line << endl;

    }
  }

  // Compile and execute the Lua script
  int error = luaL_loadstring(L, out.str().c_str());
  if (error)
  {
    cerr << lua_tostring(L, -1) << endl;
    lua_pop(L, 1);
  }

  error = lua_pcall(L, 0, 0, 0);
  if (error)
  {
    cerr << lua_tostring(L, -1) << endl;
    lua_pop(L, 1);
  }
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


