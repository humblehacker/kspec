#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <string>
#include <vector>
#include <map>
#include <boost/smart_ptr.hpp>

#include "binding.h"
#include "keyboard_visitor.h"

namespace kspec
{

using std::wstring;

class Keyboard;
class KeyMap;
class Key;

typedef std::vector<wstring> WStrings;
typedef WStrings IOPins;
typedef WStrings MatrixRow;
typedef boost::shared_ptr<MatrixRow> MatrixRowPtr;
typedef std::vector<MatrixRowPtr> Matrix;
typedef std::map<wstring, Key> Keys;
typedef std::vector<Binding::Ptr> Bindings;

/*   K E Y   */
class Key
{
public:
  Key(const wstring &location = L"ZZ") : _location(location) {}

  void add_binding(Binding::Ptr binding) { _bindings.push_back(binding); }
  void add_label(Label::Location loc, wstring label) { _labels[loc] = Label(loc, label); }

  const wstring  &location() const { return _location; }
  const Bindings &bindings() const { return _bindings; }
  const Labels   &labels()   const { return _labels;   }

  void accept(KeyboardVisitor &visitor) const;
  void accept(KeyboardExternalVisitor &visitor) const;

private:
  wstring  _location;
  Bindings _bindings;
  Labels   _labels;
};

/*   K E Y M A P   */

class KeyMap
{
public:
  KeyMap(const wstring &name = L"") : _name(name), _default(false) {}

  typedef boost::shared_ptr<KeyMap> Ptr;

  void set_base(const wstring &base)     { _base = base; }
  void make_default()                    { _default = true; }

  const wstring & name() const           { return _name; }
  const wstring & base() const           { return _base; }
  bool default_map() const               { return _default; }
  const Keys & keys() const              { return _keys; }

  Key & add_key(const wstring &location) { return _keys[location] = Key(location); }

  void accept(KeyboardVisitor &visitor) const;
  void accept(KeyboardExternalVisitor &visitor) const;

private:
  wstring _name, _base;
  bool    _default;
  Keys    _keys;
};

typedef std::map<wstring, KeyMap::Ptr> KeyMaps;

/*    L E D    */

class LED
{
public:
  enum Standard { numlock, capslock, scrolllock, compose, kana, none };
  typedef boost::shared_ptr<LED> Ptr;
  enum FlowDir { source, sink };

  LED(const wstring &ident, Standard std=none) : _name(ident), _std(std) {}

  void set_pin(const wstring &pin) { _pin = pin; }
  void set_flow(FlowDir flow) { _flow = flow; }
  void set_std(Standard std) { _std = std; }

  const wstring &name() const { return _name; }
  const wstring &pin() const  { return _pin; }
  FlowDir flow() const  { return _flow; }
  Standard std() const  { return _std; }

private:
  wstring  _name;
  wstring  _pin;
  FlowDir  _flow;
  Standard _std;
};

/*    K E Y D E F    */

class KeyDef
{
public:
  KeyDef(const wstring &ident) : _ident(ident), _is_gap(false), _has_bump(false),
    _width(1.0), _height(1.0) {}

  typedef boost::shared_ptr<KeyDef> Ptr;

  void set_width(double width)   { _width = width; }
  void set_height(double height) { _height = height; }
  void set_bump()                { _has_bump = true; }
  void set_gap()                 { _is_gap = true; }

  const wstring &ident() const   { return _ident; }
  double width() const           { return _width; }
  double height() const          { return _height; }
  bool is_gap() const            { return _is_gap; }

  void accept(KeyboardExternalVisitor &visitor) const;

private:
  wstring _ident;
  double  _width;
  double  _height;
  bool    _has_bump;
  bool    _is_gap;
};


/*    L A Y O U T    */

class Layout
{
public:
  Layout(const wstring &ident);

  typedef boost::shared_ptr<Layout> Ptr;
  typedef std::vector<KeyDef::Ptr> RowDef;
  typedef std::vector<RowDef> RowDefs;

  void add_keydef(KeyDef::Ptr keydef);
  void new_row();

  const RowDefs &rows() const { return _rows; }

  void accept(KeyboardExternalVisitor &visitor) const;

private:

  wstring _ident;
  RowDefs _rows;
};

/*    K E Y B O A R D    */

class Keyboard
{
public:
  Keyboard(const wstring &ident);

  typedef boost::shared_ptr<Keyboard> Ptr;
  typedef std::vector<LED::Ptr> LEDs;

  void set_ident(const wstring &ident)   { _ident = ident; }
  void add_col_pin(const wstring &pin)   { _cpins.push_back(pin); }
  void add_row_pin(const wstring &pin)   { _rpins.push_back(pin); }
  void add_matrix_row(MatrixRowPtr row)  { _matrix.push_back(row); }
  void add_keymap(KeyMap::Ptr map)       { _maps[map->name()] = map; }
  void add_layout(Layout::Ptr layout)    { _layout = layout; }
  void set_block_ghost_keys(bool block)  { _block_ghost_keys = block; }
  void add_led(LED::Ptr led)             { _leds.push_back(led); }

  const wstring &ident() const           { return _ident; }
  const Matrix &matrix() const           { return _matrix; }
  const IOPins &col_pins() const         { return _cpins; }
  const IOPins &row_pins() const         { return _rpins; }
  const Layout &layout() const;
  const KeyMaps &maps() const            { return _maps; }
  const LEDs &leds() const               { return _leds; }
  bool  block_ghost_keys() const         { return _block_ghost_keys; }

  void accept(KeyboardVisitor &visitor) const;
  void accept(KeyboardExternalVisitor &visitor) const;

private:
  wstring     _ident;
  Matrix      _matrix;
  IOPins      _cpins, _rpins;
  KeyMaps     _maps;
  Layout::Ptr _layout;
  bool        _block_ghost_keys;
  LEDs        _leds;
};

}; // namespace kspec

std::wostream & operator << (std::wostream &os, const kspec::WStrings &strs);


#endif // __KEYBOARD_H__
