#include <string>
#include <iostream>
#include "lua_helpers.h"
#include "utils.h"

using std::cout;
using std::endl;

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
  if (value.empty())
    lua_pushnil(L);
  else
    lua_pushstring(L, value.c_str());
  lua_setfield(L, -2, key.c_str());
}

void set_field(lua_State *L, const std::string &key, const char *value)
{
  assert(lua_istable(L, -1));
  if (strlen(value) == 0)
    lua_pushnil(L);
  else
    lua_pushstring(L, value);
  lua_setfield(L, -2, key.c_str());
}

void set_field(lua_State *L, const std::string &key, const std::wstring &value)
{
  assert(lua_istable(L, -1));
  if (value.empty())
    lua_pushnil(L);
  else
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
  if (value.empty())
    lua_pushnil(L);
  else
    lua_pushstring(L, value.c_str());
  lua_settable(L, -3);
}

void set_field(lua_State *L, int key, const char *value)
{
  assert(lua_istable(L, -1));
  lua_pushnumber(L, key);
  if (strlen(value) == 0)
    lua_pushnil(L);
  else
    lua_pushstring(L, value);
  lua_settable(L, -3);
}

void set_field(lua_State *L, int key, const std::wstring &value)
{
  assert(lua_istable(L, -1));
  lua_pushnumber(L, key);
  if (value.empty())
    lua_pushnil(L);
  else
    lua_pushstring(L, wstring_to_string(value).c_str());
  lua_settable(L, -3);
}

void
new_lua_global(lua_State *L, bool value, const std::string &name)
{
  lua_pushboolean(L, value);
  lua_setglobal(L, name.c_str());
}

