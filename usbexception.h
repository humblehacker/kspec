#ifndef __USBEXCEPTION_H__
#define __USBEXCEPTION_H__

#include <stdexcept>

class USBException : public std::runtime_error
{
public:
  USBException(int error_code)
    : std::runtime_error("USB Exception"), _error_code(error_code) {}

  virtual const char *what() const throw();

private:
  int _error_code;
};

#endif // __USBEXCEPTION_H__
