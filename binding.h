#ifndef __BINDING_H__
#define __BINDING_H__

#include <map>
#include <vector>
#include <boost/smart_ptr.hpp>
#include "boost/pointer_cast.hpp"
#include "hid_usages.h"
#include "keyboard_visitor.h"

namespace kspec
{
using std::wstring;

class Label
{
public:
  enum Location
  {
    top_left, top_center, top_right,
    center_left, center, center_right,
    bottom_left, bottom_center, bottom_right
  };
  Label(Location loc, const wstring &value) : _loc(loc), _value(value) {}
  Label() {}

  const wstring &value() const      { return _value; }
  Location       location() const   { return _loc; }
  wstring        loc_as_str() const;

  void accept(KeyboardVisitor &visitor) const;
  void accept(KeyboardExternalVisitor &visitor) const;

private:
  Location _loc;
  wstring  _value;
};

typedef std::map<Label::Location, Label> Labels;

enum Modifier
{
  left_control  = (1<<0),
  left_shift    = (1<<1),
  left_alt      = (1<<2),
  left_gui      = (1<<3),
  right_control = (1<<4),
  right_shift   = (1<<5),
  right_alt     = (1<<6),
  right_gui     = (1<<7),
  any_control   = (1<<8),
  any_shift     = (1<<9),
  any_alt       = (1<<10),
  any_gui       = (1<<11)
};

class Modifiers
{
public:
  Modifiers() : _mods(0) {}
  Modifiers &operator |=(Modifier mod) { _mods |= (1<<mod); return *this; }

  bool has_any() const { return _mods >= any_control; }

private:
  int _mods;
};

inline
const char *
mod_to_string(Modifier mod)
{
  switch (mod)
  {
  case left_shift:    return "left_shift";
  case left_alt:      return "left_alt";
  case left_control:  return "left_control";
  case left_gui:      return "left_gui";
  case right_shift:   return "right_shift";
  case right_alt:     return "right_alt";
  case right_control: return "right_control";
  case right_gui:     return "right_gui";
  case any_shift:     return "shift";
  case any_alt:       return "alt";
  case any_control:   return "control";
  case any_gui:       return "gui";
  }
}

typedef std::vector<Modifier> Mods;

class Binding
{
public:
  virtual ~Binding();

  typedef boost::shared_ptr<Binding> Ptr;

  void set_premods(int premods) { _premods = premods; }
  void add_label(Label::Location loc, wstring text) { _labels[loc] = Label(loc, text); }

  int premods() const { return _premods; }

  virtual void accept(KeyboardVisitor &visitor) const;
  virtual void accept(KeyboardExternalVisitor &visitor) const;

private:
  int    _premods;
  Labels _labels;
};

class Map : public Binding
{
public:
  Map(const Usage &usage, int mods) : _usage(usage), _mods(mods) {}
  virtual ~Map();

  typedef boost::shared_ptr<Map> Ptr;

  void set_mods(int mods) { _mods = mods; }
  void set_page(const std::wstring &page) { _page = page; }

  const Usage &usage() const { return _usage; }
  int mods() const { return _mods; }

  void accept(KeyboardVisitor &visitor) const;
  void accept(KeyboardExternalVisitor &visitor) const;

private:
  Usage        _usage;
  int          _mods;
  std::wstring _page;
};

typedef std::vector<Map::Ptr> Maps;

class Macro : public Binding
{
public:
  virtual ~Macro();

  typedef boost::shared_ptr<Macro> Ptr;

  void add_map(Map::Ptr map) { _maps.push_back(map); }
  const Maps &maps() const { return _maps; }

  void accept(KeyboardVisitor &visitor) const;
  void accept(KeyboardExternalVisitor &visitor) const;

private:
  Maps  _maps;
};

class Mode : public Binding
{
public:
  Mode(const std::wstring & name) : _name(name) {}
  virtual ~Mode();
  enum Type { momentary, toggle };

  typedef boost::shared_ptr<Mode> Ptr;

  void set_type(Type type) { _type = type; }

  const std::wstring &name() const { return _name; }
  Type type() const { return _type; }

  void accept(KeyboardVisitor &visitor) const;
  void accept(KeyboardExternalVisitor &visitor) const;

private:
  std::wstring _name;
  Type         _type;
};

}

#endif // __BINDING_H__
