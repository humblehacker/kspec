#include <sstream>
#include <iostream>
#include <stdexcept>
#include "utils.h"
#include "keyboard.h"

namespace kspec
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

  if (_layout)
    visitor.visit(*_layout);

  foreach(const KeyMaps::value_type &keymap, _maps)
    keymap.second->accept(visitor);
}

void
Keyboard::
accept(KeyboardExternalVisitor &visitor) const
{
  visitor.visit(*this);
}

const Layout &
Keyboard::
layout() const
{
 if (!_layout)
   throw std::runtime_error("No Layout has been defined.");
 return *_layout;
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

Layout::
Layout(const wstring &ident) : _ident(ident)
{
}

void
Layout::
add_keydef(KeyDef::Ptr keydef)
{
  _rows.back().push_back(keydef);
}

void
Layout::
new_row()
{
  _rows.resize(_rows.size() + 1);
}

void
Layout::
accept(KeyboardExternalVisitor &visitor) const
{
  visitor.visit(*this);
}

} // namespace kspec

std::wostream &
operator << (std::wostream &os, const kspec::WStrings &strs)
{
  os << "[ ";
  for (kspec::WStrings::const_iterator i = strs.begin(); i != strs.end(); ++i)
    os << *i << " ";
  os << "]";
  return os;
}

