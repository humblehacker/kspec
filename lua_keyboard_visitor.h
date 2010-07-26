#ifndef __LUA_KEYBOARD_VISITOR_H__
#define __LUA_KEYBOARD_VISITOR_H__

#include "keyboard_visitor.h"

struct lua_State;

namespace hh
{

class LuaKeyboardVisitor : public KeyboardExternalVisitor
{
public:
  LuaKeyboardVisitor(lua_State *L) : _L(L) {}

  virtual void visit(const Keyboard & kb);
  virtual void visit(const KeyMap & keymap);
  virtual void visit(const Key & key);
  virtual void visit(const Label & label);
  virtual void visit(const Map & map);
  virtual void visit(const Macro & macro);
  virtual void visit(const Mode & mode);

private:
  lua_State *_L;
};

} // namespace hh

#endif // __LUA_KEYBOARD_VISITOR_H__
