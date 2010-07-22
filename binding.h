#ifndef __BINDING_H__
#define __BINDING_H__

namespace hh
{
using std::wstring;

enum LabelLocation
{
  top_left, top_center, top_right,
  center_left, center, center_right,
  bottom_left, bottom_center, bottom_right
};

typedef std::map<LabelLocation, wstring> Labels;

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
  virtual ~Binding() {}

  void add_label(LabelLocation loc, wstring text) { _labels[loc] = text; }

private:
  Labels _labels;
};

class Map : public Binding
{
public:
  Map(const std::wstring &usage) : _usage(usage) {}

  void set_mods(const Mods &mods) { _mods = mods; }
  void set_page(const std::wstring &page) { _page = page; }

private:
  std::wstring _usage;
  Mods         _mods;
  std::wstring _page;
};

class Macro : public Binding
{
public:
  void add_map(Map *map) { _maps.push_back(map); }

private:
  typedef std::vector<Map*> Maps;
  Maps  _maps;
};

class Mode : public Binding
{
public:
  Mode(const std::wstring & name) : _name(name) {}

private:
  std::wstring _name;
};

}

#endif // __BINDING_H__
