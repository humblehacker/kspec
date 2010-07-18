#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <string>
#include <vector>

namespace hh
{

class Cell;

typedef std::vector<std::wstring> WStrings;
typedef WStrings IOPins;
typedef WStrings MatrixRow;
typedef std::vector<MatrixRow> Matrix;


class Keyboard
{
public:
  Keyboard(const std::wstring &ident);

  void set_ident(const std::wstring &ident) { _ident = ident; }
  const std::wstring &ident() const { return _ident; }

  Matrix & matrix() { return _matrix; }
  IOPins & cpins()  { return _cpins; }
  IOPins & rpins()  { return _rpins; }

private:
  std::wstring _ident;
  Matrix       _matrix;
  IOPins       _cpins, _rpins;
};

class Cell
{
public:
  Cell(int row, int col) : _row(row), _col(col) {}

private:
  int _row, _col;
};

}; // namespace hh

std::wostream & operator << (std::wostream &os, const hh::IOPins &strs);

#endif // __KEYBOARD_H__
