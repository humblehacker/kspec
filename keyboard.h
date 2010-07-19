#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <string>
#include <vector>
#include <map>
#include <tr1/memory>

#include "binding.h"

namespace hh
{
using std::wstring;

class Keyboard;
class KeyMap;
class Key;

typedef std::tr1::shared_ptr<KeyMap> KeyMapPtr;
typedef std::vector<wstring> WStrings;
typedef WStrings IOPins;
typedef WStrings MatrixRow;
typedef std::tr1::shared_ptr<MatrixRow> MatrixRowPtr;
typedef std::vector<MatrixRowPtr> Matrix;
typedef std::map<wstring, KeyMapPtr> KeyMaps;
typedef std::map<wstring, Key> Keys;
typedef std::vector<Binding*> Bindings;
typedef std::tr1::shared_ptr<Keyboard> KeyboardPtr;

/*   K E Y   */
class Key
{
public:
  Key(const wstring &location = L"ZZ") : _location(location) {}

  void add_binding(Binding *binding) { _bindings.push_back(binding); }
  void add_label(LabelLocation loc, wstring label) { _labels[loc] = label; }

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
  void set_base(const wstring &base)     { _base = base; }
  void make_default()                         { _default = true; }

  const wstring & name() const           { return _name; }
  const wstring & base() const           { return _base; }
  bool default_map() const                    { return _default; }
  const Keys & keys() const                   { return _keys; }

  Key & add_key(const wstring &location) { return _keys[location] = Key(location); }

private:
  wstring _name, _base;
  bool         _default;
  Keys         _keys;
};

/*    K E Y B O A R D    */

class Keyboard
{
public:
  Keyboard(const wstring &ident);

  void set_ident(const wstring &ident) { _ident = ident; }
  void add_col_pin(const wstring &pin) { _cpins.push_back(pin); }
  void add_row_pin(const wstring &pin) { _rpins.push_back(pin); }
  void add_matrix_row(MatrixRowPtr row)     { _matrix.push_back(row); }
  void add_keymap(KeyMapPtr map)            { _maps[map->name()] = map; }

  const wstring &ident() const         { return _ident; }
  const Matrix &matrix() const              { return _matrix; }
  const IOPins &cpins() const               { return _cpins; }
  const IOPins &rpins() const               { return _rpins; }
  const KeyMaps &maps() const               { return _maps; }

private:
  wstring _ident;
  Matrix       _matrix;
  IOPins       _cpins, _rpins;
  KeyMaps      _maps;
};

}; // namespace hh

std::wostream & operator << (std::wostream &os, const hh::WStrings &strs);

#endif // __KEYBOARD_H__
