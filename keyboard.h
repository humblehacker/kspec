#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <string>
#include <vector>
#include <map>

namespace hh
{

class Cell;
class KeyMap;
class Key;

typedef std::vector<std::wstring> WStrings;
typedef WStrings IOPins;
typedef WStrings MatrixRow;
typedef std::vector<MatrixRow> Matrix;
typedef std::map<std::wstring, KeyMap> KeyMaps;
typedef std::map<std::wstring, Key> Keys;

/*   K E Y   */
class Key
{
public:
  Key(const std::wstring &location = L"ZZ") : _location(location) {}

private:
  std::wstring _location;
};

/*   K E Y M A P   */
class KeyMap
{
public:
  KeyMap(const std::wstring &name = L"") : _name(name), _default(false) {}
  void set_base(const std::wstring &base) { _base = base; }
  void make_default() { _default = true; }

  const std::wstring & name() const { return _name; }
  const std::wstring & base() const { return _base; }
  bool default_map() const { return _default; }
  const Keys & keys() const { return _keys; }

  Key & add_key(const std::wstring &location) { return _keys[location] = Key(location); }

private:
  std::wstring _name, _base;
  bool         _default;
  Keys         _keys;
};

/*   C E L L   */
class Cell
{
public:
  Cell(int row, int col) : _row(row), _col(col) {}

private:
  int _row, _col;
};

/*    K E Y B O A R D    */

class Keyboard
{
public:
  Keyboard(const std::wstring &ident);

  void set_ident(const std::wstring &ident) { _ident = ident; }
  const std::wstring &ident() const { return _ident; }

  Matrix & matrix() { return _matrix; }
  IOPins & cpins()  { return _cpins; }
  IOPins & rpins()  { return _rpins; }
  const KeyMaps &maps() const { return _maps; }

  KeyMap& new_map(const std::wstring &name) { return _maps[name] = KeyMap(name); }

private:
  std::wstring _ident;
  Matrix       _matrix;
  IOPins       _cpins, _rpins;
  KeyMaps      _maps;
};

}; // namespace hh

std::wostream & operator << (std::wostream &os, const hh::WStrings &strs);

#endif // __KEYBOARD_H__
