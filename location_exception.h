#ifndef __LOCATION_EXCEPTION_H__
#define __LOCATION_EXCEPTION_H__

#include "streamable_exception.h"
#include "location.hh"

class location_exception : public streamable_exception
{
public:
  location_exception(const yy::location &loc) : _loc(loc) {}
  location_exception(const location_exception &o)
  : streamable_exception(o), _loc(o._loc) {}
  ~location_exception() throw () {}

  const yy::location &loc() const { return _loc; }

  template <typename T>
  const location_exception &operator<<(const T &rhs) const
  {
    stream << "kspec: "
           << _loc.end.filename << ":"
           << _loc.end.line     << ": ";
    streamable_exception::operator<<(rhs);
    return *this;
  }

protected:
  yy::location _loc;
};

inline
std::ostream &
operator<<(std::ostream &os, const location_exception &rhs)
{
  os << rhs.loc() << " " << rhs.str();
  return os;
}

#endif // __LOCATION_EXCEPTION_H__
