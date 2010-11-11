#include "binding.h"
#include "utils.h"

namespace kspec
{

wstring
Label::
loc_as_str() const
{
  switch (_loc)
  {
  case top_left:      return L"top_left";
  case top_center:    return L"top_center";
  case top_right:     return L"top_right";
  case center_left:   return L"center_left";
  case center:        return L"center";
  case center_right:  return L"center_right";
  case bottom_left:   return L"bottom_left";
  case bottom_center: return L"bottom_center";
  case bottom_right:  return L"bottom_right";
  }
  return L"";
}

void
Label::
accept(KeyboardVisitor &visitor) const
{
  visitor.visit(*this);
}

void
Label::
accept(KeyboardExternalVisitor &visitor) const
{
  visitor.visit(*this);
}

Binding::
~Binding()
{
}

void
Binding::
accept(KeyboardVisitor &visitor) const
{
  visitor.visit(*this);
}

void
Binding::
accept(KeyboardExternalVisitor &visitor) const
{
  visitor.visit(*this);
}

Macro::
~Macro()
{
}

void
Macro::
accept(KeyboardVisitor &visitor) const
{
  visitor.visit(*this);
  foreach(const Maps::value_type &map, _maps)
    map->accept(visitor);
}

void
Macro::
accept(KeyboardExternalVisitor &visitor) const
{
  visitor.visit(*this);
}

Map::
~Map()
{
}

void
Map::
accept(KeyboardVisitor &visitor) const
{
  visitor.visit(*this);
}

void
Map::
accept(KeyboardExternalVisitor &visitor) const
{
  visitor.visit(*this);
}

Mode::
~Mode()
{
}

void
Mode::
accept(KeyboardVisitor &visitor) const
{
  visitor.visit(*this);
}

void
Mode::
accept(KeyboardExternalVisitor &visitor) const
{
  visitor.visit(*this);
}

} // namespace kspec
