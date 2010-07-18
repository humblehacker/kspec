#include <sstream>
#include <iostream>
#include "utils.h"
#include "keyboard.h"

namespace hh
{

Keyboard::
Keyboard(const std::wstring &ident) : _ident(ident)
{
}

}

std::wostream &
operator << (std::wostream &os, const hh::WStrings &strs)
{
  os << "[ ";
  for (hh::WStrings::const_iterator i = strs.begin(); i != strs.end(); ++i)
    os << *i << " ";
  os << "]";
  return os;
}

