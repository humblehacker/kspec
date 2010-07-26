#include "lua_keyboard_visitor.h"
#include "lua_helpers.h"
#include "keyboard.h"

#include <iostream>

using std::wcout;
using std::cout;
using std::endl;

namespace hh
{

void
LuaKeyboardVisitor::
visit(const hh::Keyboard &kb)
{
  assert(lua_istable(_L, -1));

  // kb.matrix = { row_count = XX, col_count = XX }
  lua_newtable(_L);
  set_field(_L, "row_count", (int)kb.matrix().size());
  set_field(_L, "col_count", (int)kb.matrix().front()->size());
  lua_setfield(_L, -2, "matrix");

  // kb.block_ghost_keys = <boolean>
  set_field(_L, "block_ghost_keys", kb.block_ghost_keys());

  // kb.row_pins = { "XX", "XX", ... }
  set_array(_L, "row_pins", kb.row_pins());

  // kb.col_pins = { "XX", "XX", ... }
  set_array(_L, "col_pins", kb.col_pins());

  // kb.maps = { keymap1, keymap2, ... }
  new_array(_L);
  foreach(const KeyMaps::value_type &keymap, kb.maps())
  {
    lua_newtable(_L);
    keymap.second->accept(*this);
    array_append(_L);
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

  // keymap.keys = { key1, key2, ... }
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
  foreach(const Binding::Ptr &binding, key.bindings())
    binding->accept(*this);
  lua_setfield(_L, -2, "bindings");
}

void
LuaKeyboardVisitor::
visit(const hh::Map & map)
{
  assert(lua_istable(_L, -1));

  lua_pushlightuserdata(_L, const_cast<void*>(static_cast<const void*>(&map.usage())));
  lua_setfield(_L, -2, "usage");
}

void
LuaKeyboardVisitor::
visit(const hh::Macro & macro)
{
  assert(lua_istable(_L, -1));
}

void
LuaKeyboardVisitor::
visit(const hh::Mode & mode)
{
  assert(lua_istable(_L, -1));
}

void
LuaKeyboardVisitor::
visit(const hh::Label &label)
{
  assert(lua_istable(_L, -1));
  set_field(_L, wstring_to_string(label.loc_as_str()), label.value());
}

} // namespace hh
