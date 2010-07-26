#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <string>
#include <vector>
#include <map>
#include <tr1/memory>

#include "binding.h"
#include "keyboard_visitor.h"

namespace hh
{
using std::wstring;

class Keyboard;
class KeyMap;
class Key;

typedef std::vector<wstring> WStrings;
typedef WStrings IOPins;
typedef WStrings MatrixRow;
typedef std::tr1::shared_ptr<MatrixRow> MatrixRowPtr;
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
  const Labels   &labels() const   { return _labels;   }

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

/*    K E Y B O A R D    */

class Keyboard
{
public:
  Keyboard(const wstring &ident);

  typedef boost::shared_ptr<Keyboard> Ptr;

  void set_ident(const wstring &ident)   { _ident = ident; }
  void add_col_pin(const wstring &pin)   { _cpins.push_back(pin); }
  void add_row_pin(const wstring &pin)   { _rpins.push_back(pin); }
  void add_matrix_row(MatrixRowPtr row)  { _matrix.push_back(row); }
  void add_keymap(KeyMap::Ptr map)       { _maps[map->name()] = map; }
  void set_block_ghost_keys(bool block)  { _block_ghost_keys = block; }

  const wstring &ident() const           { return _ident; }
  const Matrix &matrix() const           { return _matrix; }
  const IOPins &col_pins() const         { return _cpins; }
  const IOPins &row_pins() const         { return _rpins; }
  const KeyMaps &maps() const            { return _maps; }
  bool  block_ghost_keys() const         { return _block_ghost_keys; }

  void accept(KeyboardVisitor &visitor) const;
  void accept(KeyboardExternalVisitor &visitor) const;

private:
  wstring _ident;
  Matrix  _matrix;
  IOPins  _cpins, _rpins;
  KeyMaps _maps;
  bool    _block_ghost_keys;
};

}; // namespace hh

std::wostream & operator << (std::wostream &os, const hh::WStrings &strs);


#endif // __KEYBOARD_H__
