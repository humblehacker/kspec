#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "generate.h"

using namespace std;

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
    cout << endl;
    cout << "====================================================================" << endl;
    cout << template_filename.string()                                             << endl;
    cout << "====================================================================" << endl;
    cout << endl;
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
  lua_newtable(L);
  hh::LuaKeyboardVisitor v(L);
  kb.accept(v);
  lua_setglobal(L, "kb");
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


