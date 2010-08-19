#include <string>
#include <libusb-1.0/libusb.h>
#include "usbexception.h"

using namespace std;

const char *
USBException::
what() const throw()
{
  static std::string msg(std::runtime_error::what());
  switch (_error_code)
  {
    case LIBUSB_SUCCESS:
      msg += " : Success (no error)";
      break;

    case LIBUSB_ERROR_IO:
      msg += " : Input/output error";
      break;

    case LIBUSB_ERROR_INVALID_PARAM:
      msg += " : Invalid parameter";
      break;

    case LIBUSB_ERROR_ACCESS:
      msg += " : Access denied (insufficient permissions)";
      break;

    case LIBUSB_ERROR_NO_DEVICE:
      msg += " : No such device (it may have been disconnected)";
      break;

    case LIBUSB_ERROR_NOT_FOUND:
      msg += " : Entity not found";
      break;

    case LIBUSB_ERROR_BUSY:
      msg += " : Resource busy";
      break;

    case LIBUSB_ERROR_TIMEOUT:
      msg += " : Operation timed out";
      break;

    case LIBUSB_ERROR_OVERFLOW:
      msg += " : Overflow";
      break;

    case LIBUSB_ERROR_PIPE:
      msg += " : Pipe error";
      break;

    case LIBUSB_ERROR_INTERRUPTED:
      msg += " : System call interrupted (perhaps due to signal)";
      break;

    case LIBUSB_ERROR_NO_MEM:
      msg += " : Insufficient memory";
      break;

    case LIBUSB_ERROR_NOT_SUPPORTED:
      msg += " : Operation not supported or unimplemented on this platform";
      break;

    case LIBUSB_ERROR_OTHER:
      msg += " : Other error";
      break;
  }
  return msg.c_str();
}

