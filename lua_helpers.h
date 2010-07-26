#ifndef __LUA_HELPERS_H__
#define __LUA_HELPERS_H__

#include "boost/foreach.hpp"
#if !defined(foreach)
#define foreach BOOST_FOREACH
#endif
#include <cassert>

extern "C"
{
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
}

#include "utils.h"

//extern "C" {#include "../lua-5.1.4/src/lstate.h"}
//inline int lua_stacksize(lua_State *L)  { return (L->top - L->base); }

void set_field(lua_State *L, const std::string &key, bool value);
void set_field(lua_State *L, const std::string &key, int value);
void set_field(lua_State *L, const std::string &key, const std::string &value);
void set_field(lua_State *L, const std::string &key, const std::wstring &value);
void set_field(lua_State *L, int key, bool value);
void set_field(lua_State *L, int key, int value);
void set_field(lua_State *L, int key, const std::string &value);
void set_field(lua_State *L, int key, const std::wstring &value);

void new_array(lua_State *L);
template <typename ValueT>
void append_item(lua_State *L, const ValueT &value)
{
  // get the next index
  lua_getfield(L, -1, "_last");
  int index = lua_tonumber(L, -1) + 1;
  lua_pop(L, 1);

  // append the new value
  set_field(L, index, value);

  // update the last index
  set_field(L, "_last", index);
}

void array_append(lua_State *L);

inline
const char *
to_cstring(const std::wstring &val)
{
  return wstring_to_string(val).c_str();
}

inline
const char *
to_cstring(const std::string &val)
{
  return val.c_str();
}

inline
const std::string
to_string(const std::wstring &val)
{
  return wstring_to_string(val);
}

inline
const std::string
to_string(const std::string &val)
{
  return val;
}

template <typename ArrayT>
void
set_array(lua_State *L, const std::string &name, const ArrayT& source)
{
  assert(lua_istable(L, -1));

  lua_newtable(L);
  int index = 1;
  foreach(const typename ArrayT::value_type &val, source)
  {
    set_field(L, index, to_string(val));
    ++index;
  }
  lua_setfield(L, -2, name.c_str());
}

void new_lua_global(lua_State *L, bool value, const std::string &name);


#endif // __LUA_HELPERS_H__
