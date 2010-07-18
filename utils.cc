#include "utils.h"

const std::string &
wstring_to_string(const std::wstring &src)
{
  static std::string dst;
  dst.resize(src.length(), ' ');
  std::copy(src.begin(), src.end(), dst.begin());
  return dst;
}

const std::string &
wstring_to_string(const wchar_t *src)
{
  const std::wstring temp(src);
  return wstring_to_string(temp);
}

