#include <iostream>

#include "usb.h"
#include "utils.h"

using namespace std;

//  class USBTransfer ===========================================

USBTransfer::
USBTransfer(int iso_packets) : _transfer(libusb_alloc_transfer(iso_packets))
{
  if (!_transfer)
    throw USBException(LIBUSB_ERROR_NO_MEM);
}

USBTransfer::
~USBTransfer()
{
  libusb_free_transfer(_transfer);
}

void
USBTransfer::
submit()
{
  int result = libusb_submit_transfer(_transfer);
  if (result != LIBUSB_SUCCESS)
    throw USBException(result);
}

void
USBTransfer::
cancel()
{
  int result = libusb_cancel_transfer(_transfer);
  if (result != LIBUSB_SUCCESS)
    throw USBException(result);
}

//  class USBBulkTransfer =======================================

USBBulkTransfer::
USBBulkTransfer(USBDevice::Ptr device, unsigned char endpoint, int buffer_size,
                libusb_transfer_cb_fn callback, unsigned int timeout)
: USBTransfer(), _buffer(buffer_size)
{
  libusb_fill_bulk_transfer(_transfer, device->handle(), endpoint,
    &_buffer[0], _buffer.size(), callback, NULL, timeout);
}

//  class USBDevice =============================================

USBDevice::
USBDevice(libusb_context_ptr ctx, libusb_device *device)
: _context(ctx), _device(libusb_ref_device(device)), _handle(NULL)
{
}

USBDevice::
~USBDevice()
{
  close();
  libusb_unref_device(_device);
}

void
USBDevice::
open()
{
  int result = libusb_open(_device, &_handle);
  if (result != 0)
    throw USBException(result);
}

void
USBDevice::
close()
{
  release_all_interfaces();
  libusb_close(_handle);
  _handle = NULL;
}

void
USBDevice::
get_device_descriptor(libusb_device_descriptor *dev_desc)
{
  int result = libusb_get_device_descriptor(_device, dev_desc);
  if (result != LIBUSB_SUCCESS)
    throw USBException(result);
}

void
USBDevice::
claim_interface(int interface)
{
  int result;
  if (libusb_kernel_driver_active(_handle, interface))
  {
    result = libusb_detach_kernel_driver(_handle, interface);
    if (result != LIBUSB_SUCCESS)
      throw USBException(result);
  }
  result = libusb_claim_interface(_handle, interface);
  if (result != LIBUSB_SUCCESS)
    throw USBException(result);
  _claimed_interfaces.insert(interface);
}

void
USBDevice::
release_interface(int interface)
{
  if (!_claimed_interfaces.count(interface))
    return;
  int result = libusb_release_interface(_handle, interface);
  if (result != LIBUSB_SUCCESS)
    throw USBException(result);
  _claimed_interfaces.erase(interface);
}

void
USBDevice::
release_all_interfaces()
{
  // used in destructor - must not throw
  foreach(int interface, _claimed_interfaces)
  {
    int result = libusb_release_interface(_handle, interface);
    if (result != LIBUSB_SUCCESS)
    {
      cerr << "Failed to release interface " << interface << endl;
      cerr << " Error code: " << result << endl;
    }

  }
  _claimed_interfaces.clear();
}

//  class USB ===================================================

USB::
USB(int debug_level)
{
  libusb_context *context = NULL;
  int result = libusb_init(&context);
  if (result != LIBUSB_SUCCESS) throw USBException(result);
  _context.reset(context, libusb_exit);
  libusb_set_debug(context, debug_level);
}

USB::
~USB()
{
}

USBDevice::List
USB::
get_device_list()
{
  libusb_device **devices;
  ssize_t count = libusb_get_device_list(_context.get(), &devices);
  if (count < 0) throw USBException(count);

  USBDevice::List device_list(new std::vector<USBDevice::Ptr>());
  while (count--)
  {
    device_list->push_back(USBDevice::Ptr(new USBDevice(_context, devices[count])));
  }
  libusb_free_device_list(devices, 1);
  return device_list;
}

void
USB::
handle_events()
{
  libusb_handle_events(_context.get());
}
