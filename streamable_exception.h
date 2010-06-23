#ifndef __STREAM_EXCEPTION_H__
#define __STREAM_EXCEPTION_H__

#include <stdexcept>
#include <sstream>

// The SUNPro compiler generates warnings anytime an
// exception of this class is thrown via its operator<<().
// Since exception objects are copied when thrown, the
// warning is spurious.
#if defined(__SUNPRO_CC)
  #pragma error_messages(off, reftotemp )
#endif

class streamable_exception : public std::exception
{
public:
  streamable_exception() {}
  streamable_exception(const streamable_exception &other)
  : stream(other.stream.str()) {}
  ~streamable_exception() throw () {}

  template <typename T>
  const streamable_exception &operator<<(const T &rhs) const
  {
    stream << rhs;
    return *this;
  }

  const char * what() const throw() { return str().c_str(); }
  const std::string &str() const { return message = stream.str(); }

protected:
  mutable std::stringstream stream;
  mutable std::string       message;
};

inline
std::ostream &
operator<<(std::ostream &os, const streamable_exception &rhs)
{
  os << rhs.str();
  return os;
}

#endif // __STREAM_EXCEPTION_H__
