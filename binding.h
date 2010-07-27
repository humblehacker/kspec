#ifndef __BINDING_H__
#define __BINDING_H__

#include <map>
#include <vector>
#include "boost/smart_ptr.hpp"
#include "boost/pointer_cast.hpp"
#include "hid_usages.h"
#include "keyboard_visitor.h"
namespace hh
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
  left_shift,
  left_alt,
  left_control,
  left_gui,
  right_shift,
  right_alt,
  right_control,
  right_gui,
  shift,
  alt,
  control,
  gui
};

typedef std::vector<Modifier> Mods;

class Binding
{
public:
  virtual ~Binding();

  typedef boost::shared_ptr<Binding> Ptr;

  void set_premods(const Mods &premods) { _premods = premods; }
  void add_label(Label::Location loc, wstring text) { _labels[loc] = Label(loc, text); }

  virtual void accept(KeyboardVisitor &visitor) const;
  virtual void accept(KeyboardExternalVisitor &visitor) const;

private:
  Mods   _premods;
  Labels _labels;
};

class Map : public Binding
{
public:
  Map(const Usage &usage, const Mods &mods) : _usage(usage), _mods(mods) {}
  virtual ~Map();

  typedef boost::shared_ptr<Map> Ptr;

  void set_mods(const Mods &mods) { _mods = mods; }
  void set_page(const std::wstring &page) { _page = page; }

  const Usage &usage() const { return _usage; }

  void accept(KeyboardVisitor &visitor) const;
  void accept(KeyboardExternalVisitor &visitor) const;

private:
  Usage        _usage;
  Mods         _mods;
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

  typedef boost::shared_ptr<Mode> Ptr;

  const std::wstring &name() const { return _name; }

  void accept(KeyboardVisitor &visitor) const;
  void accept(KeyboardExternalVisitor &visitor) const;

private:
  std::wstring _name;
};

}

#endif // __BINDING_H__
