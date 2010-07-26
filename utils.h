#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include "boost/foreach.hpp"
#if !defined(foreach)
#define foreach BOOST_FOREACH
#endif

const std::string wstring_to_string(const std::wstring &src);
const std::string wstring_to_string(const wchar_t *src);
std::wstring unslash(const std::wstring &input);

template <typename S>
const S &
slash(const S &what)
{
  const S escaped = "\\\"\'\n\t\r";
  static S result;
  result = what;
  size_t look_here = 0;
  size_t found_here;
  while ((found_here = result.find_first_of(escaped, look_here)) != S::npos)
  {
    result.insert(found_here, 1, '\\');
    look_here = found_here + 2;
    switch (result[look_here])
    {
    case '\n': result[look_here] = 'n'; break;
    case '\t': result[look_here] = 't'; break;
    case '\r': result[look_here] = 'r'; break;
    }
  }
  return result;
}

//template <typename StringT>



#endif // __UTILS_H__
