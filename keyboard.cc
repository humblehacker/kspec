#include <sstream>
#include <iostream>
#include "utils.h"
#include "keyboard.h"

namespace hh
{

Keyboard::
Keyboard(const wstring &ident) : _ident(ident)
{
}

void
Keyboard::
accept(KeyboardVisitor &visitor) const
{
  visitor.visit(*this);
  foreach(const KeyMaps::value_type &keymap, _maps)
    keymap.second->accept(visitor);
}

void
Keyboard::
accept(KeyboardExternalVisitor &visitor) const
{
  visitor.visit(*this);
}

void
KeyMap::
accept(KeyboardVisitor &visitor) const
{
  visitor.visit(*this);
  foreach(const Keys::value_type &key, _keys)
    key.second.accept(visitor);
}

void
KeyMap::
accept(KeyboardExternalVisitor &visitor) const
{
  visitor.visit(*this);
}

void
Key::
accept(KeyboardVisitor &visitor) const
{
  visitor.visit(*this);
  foreach(const Labels::value_type &pair, _labels)
    pair.second.accept(visitor);
  foreach(const Binding::Ptr &binding, _bindings)
    binding->accept(visitor);
}

void
Key::
accept(KeyboardExternalVisitor &visitor) const
{
  visitor.visit(*this);
}

} // namespace hh

std::wostream &
operator << (std::wostream &os, const hh::WStrings &strs)
{
  os << "[ ";
  for (hh::WStrings::const_iterator i = strs.begin(); i != strs.end(); ++i)
    os << *i << " ";
  os << "]";
  return os;
}

