#ifndef __GENERATE_H__
#define __GENERATE_H__

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

#include "lua_helpers.h"
#include "lua_keyboard_visitor.h"
#include "keyboard.h"
#include "options.h"

void build_lua_environment(const kspec::Keyboard &kb, lua_State *L);
void generate(const fs::path &template_filename, const fs::path &output_dir,
              lua_State *L, const po::variables_map &options);
void generate_code(po::variables_map &options, kspec::Keyboard::Ptr &kb);
void go_interactive(lua_State *L);

#endif // __GENERATE_H__
