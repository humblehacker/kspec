#include "lua_keyboard_visitor.h"
#include "lua_helpers.h"
#include "keyboard.h"

#include <iostream>
#include <iomanip>

using std::wcout;
using std::cout;
using std::endl;
using std::hex;

namespace hh
{

void
LuaKeyboardVisitor::
visit(const hh::Keyboard &kb)
{
  assert(lua_istable(_L, -1));

  // kb.name = ""
  set_field(_L, "name", kb.ident());

  // kb.matrix = { row_count = XX, col_count = XX,
  //               rows = { { XX, XX, ... }, { XX, XX, XX, ... } } }
  lua_newtable(_L);
  set_field(_L, "row_count", (int)kb.matrix().size());
  set_field(_L, "col_count", (int)kb.matrix().front()->size());
  lua_newtable(_L);
  int row_index = 1, col_index;
  foreach(const MatrixRowPtr &row, kb.matrix())
  {
    col_index = 1;
    lua_pushnumber(_L, row_index++);
    lua_newtable(_L);
    foreach(const wstring &location, *row)
    {
      lua_pushnumber(_L, col_index++);
      lua_pushstring(_L, wstring_to_string(location).c_str());
      lua_settable(_L, -3);
    }
    lua_settable(_L, -3);
  }
  lua_setfield(_L, -2, "rows");
  lua_setfield(_L, -2, "matrix");

  // kb.block_ghost_keys = <boolean>
  set_field(_L, "block_ghost_keys", kb.block_ghost_keys());

  // kb.row_pins = { "XX", "XX", ... }
  set_array(_L, "row_pins", kb.row_pins());

  // kb.col_pins = { "XX", "XX", ... }
  set_array(_L, "col_pins", kb.col_pins());

  // kb.keymaps = { "name1" = keymap1, "name2" = keymap2, ... }
  lua_newtable(_L);
  foreach(const KeyMaps::value_type &keymap, kb.maps())
  {
    lua_pushstring(_L, wstring_to_string(keymap.first).c_str());
    lua_newtable(_L);
    keymap.second->accept(*this);
    lua_settable(_L, -3);
  }
  lua_setfield(_L, -2, "keymaps");
}

void
LuaKeyboardVisitor::
visit(const hh::KeyMap & keymap)
{
  assert(lua_istable(_L, -1));
//wcout << "KeyMap: " << keymap.name() << endl;

  // keymap.name = ""
  set_field(_L, "name", keymap.name());

  // keymap.base = ""
  set_field(_L, "base", keymap.base());

  // keymap.is_default = <boolean>
  set_field(_L, "is_default", keymap.default_map());

  // keymap.keys = { "location2" = key1, "location2" = key2, ... }
  lua_newtable(_L);
  foreach(const Keys::value_type &key, keymap.keys())
  {
    lua_pushstring(_L, wstring_to_string(key.first).c_str());
    lua_newtable(_L);
    key.second.accept(*this);
    lua_settable(_L, -3);
  }
  lua_setfield(_L, -2, "keys");
}

void
LuaKeyboardVisitor::
visit(const hh::Key & key)
{
  assert(lua_istable(_L, -1));
//wcout << "Key: " << key.location() << endl;

  // key.location = ""
  set_field(_L, "location", key.location());

  // key.labels = { where = "what", where2 = "what2" }
  lua_newtable(_L);
  foreach(const hh::Labels::value_type &pair, key.labels())
    pair.second.accept(*this);
  lua_setfield(_L, -2, "labels");

  // key.bindings = { binding1, binding2, ... }
  lua_newtable(_L);
  int array_index = 1;
  foreach(const Binding::Ptr &binding, key.bindings())
  {
    lua_pushnumber(_L, array_index++);
    lua_newtable(_L);
    // binding.premods.stdmods = low 8 bits
    // binding.premods.anymods = high 4 bits
    lua_newtable(_L);
    int stdmods = binding->premods() & 0xFF;
    int anymods = binding->premods() & 0xF00;
    set_field(_L, "stdmods", stdmods);
    set_field(_L, "anymods", (anymods>>4)|(anymods>>8));
    lua_setfield(_L, -2, "premods");
    binding->accept(*this);
    lua_settable(_L, -3);
  }
  lua_setfield(_L, -2, "bindings");
}

void
LuaKeyboardVisitor::
visit(const hh::Map & map)
{
  assert(lua_istable(_L, -1));
//wcout << "Map: " << endl;

  // map.class = "Map"
  set_field(_L, "class", "Map");

  // map.usage.name = ""
  lua_newtable(_L);
  set_field(_L, "name", map.usage().key);
  set_field(_L, "is_modifier",  map.usage().is_modifier());
  lua_setfield(_L, -2, "usage");

  // map.modifiers = <number>
  set_field(_L, "modifiers", map.mods());

//lua_pushlightuserdata(_L, const_cast<void*>(static_cast<const void*>(&map.usage())));
//lua_setfield(_L, -2, "usage");
}

void
LuaKeyboardVisitor::
visit(const hh::Macro & macro)
{
  assert(lua_istable(_L, -1));
//wcout << "Macro: " << endl;

  set_field(_L, "class", "Macro");

  // macro.maps = { map1, map2, ... }
  lua_newtable(_L);
  int array_index = 1;
  foreach(const Map::Ptr &map, macro.maps())
  {
    lua_pushnumber(_L, array_index++);
    lua_newtable(_L);
    map->accept(*this);
    lua_settable(_L, -3);
  }
  lua_setfield(_L, -2, "maps");
}

void
LuaKeyboardVisitor::
visit(const hh::Mode & mode)
{
  assert(lua_istable(_L, -1));
//wcout << "Mode: " << endl;

  set_field(_L, "class", "Mode");
  set_field(_L, "name", mode.name());
  if (mode.type() == hh::Mode::momentary)
    set_field(_L, "type", "MOMENTARY");
  else if (mode.type() == hh::Mode::toggle)
    set_field(_L, "type", "TOGGLE");
}

void
LuaKeyboardVisitor::
visit(const hh::Label &label)
{
  assert(lua_istable(_L, -1));
  set_field(_L, wstring_to_string(label.loc_as_str()), label.value());
}

} // namespace hh
