#include <string>
#include <iostream>
#include "lua_helpers.h"
#include "utils.h"

using std::cout;
using std::endl;

void
new_array(lua_State *L)
{
  lua_newtable(L);
  set_field(L, "_last", 0);
}

void array_append(lua_State *L)
{
  assert(lua_istable(L, -2));

  // get the next index
  lua_getfield(L, -2, "_last");
  int index = lua_tonumber(L, -1) + 1;
  cout << index << endl;

  // append the new value
  lua_insert(L, -2);
  assert(lua_isnumber(L, -2));
  assert(lua_istable(L, -3));
  lua_settable(L, -3);

  // update the last index
  lua_pushnumber(L, index);
  lua_setfield(L, -2, "_last");
}

void set_field(lua_State *L, const std::string &key, bool value)
{
  assert(lua_istable(L, -1));
  lua_pushboolean(L, value);
  lua_setfield(L, -2, key.c_str());
}

void set_field(lua_State *L, const std::string &key, int value)
{
  assert(lua_istable(L, -1));
  lua_pushnumber(L, value);
  lua_setfield(L, -2, key.c_str());
}

void set_field(lua_State *L, const std::string &key, const std::string &value)
{
  assert(lua_istable(L, -1));
  lua_pushstring(L, value.c_str());
  lua_setfield(L, -2, key.c_str());
}

void set_field(lua_State *L, const std::string &key, const std::wstring &value)
{
  assert(lua_istable(L, -1));
  lua_pushstring(L, wstring_to_string(value).c_str());
  lua_setfield(L, -2, key.c_str());
}

void set_field(lua_State *L, int key, bool value)
{
  assert(lua_istable(L, -1));
  lua_pushnumber(L, key);
  lua_pushboolean(L, value);
  lua_settable(L, -3);
}

void set_field(lua_State *L, int key, int value)
{
  assert(lua_istable(L, -1));
  lua_pushnumber(L, key);
  lua_pushnumber(L, value);
  lua_settable(L, -3);
}

void set_field(lua_State *L, int key, const std::string &value)
{
  assert(lua_istable(L, -1));
  lua_pushnumber(L, key);
  lua_pushstring(L, value.c_str());
  lua_settable(L, -3);
}

void set_field(lua_State *L, int key, const std::wstring &value)
{
  assert(lua_istable(L, -1));
  lua_pushnumber(L, key);
  lua_pushstring(L, wstring_to_string(value).c_str());
  lua_settable(L, -3);
}

void
new_lua_global(lua_State *L, bool value, const std::string &name)
{
  lua_pushboolean(L, value);
  lua_setglobal(L, name.c_str());
}

