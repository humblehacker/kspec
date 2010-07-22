#include "utils.h"

using std::string;
using std::wstring;

const string &
wstring_to_string(const wstring &src)
{
  static string dst;
  dst.resize(src.length(), ' ');
  std::copy(src.begin(), src.end(), dst.begin());
  return dst;
}

const string &
wstring_to_string(const wchar_t *src)
{
  const wstring temp(src);
  return wstring_to_string(temp);
}

wstring
unslash(const wstring &input)
{
  static const wstring escaped(L":<>'\"\\|");
  wstring result = input;
  size_t look_here = 0;
  size_t found_here;
  while ((found_here = result.find('\\', look_here)) != wstring::npos)
  {
    if (found_here < result.length() && escaped.find(result[found_here+1]) != wstring::npos)
      result.erase(found_here, 1);
    look_here = ++found_here;
  }
  return result;
}

