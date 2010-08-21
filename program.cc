#include <iostream>
#include <iomanip>
#include <boost/lambda/lambda.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <functional>
#include "utils.h"
#include "program.h"
#include "usb.h"

using namespace std;

void
printdev(libusb_device *dev)
{
  libusb_device_descriptor desc;
  int r = libusb_get_device_descriptor(dev, &desc);
  if (r < 0)
  {
    cout << "failed to get device descriptor" << endl;
    return;
  }
  cout << "Number of possible configurations: " << (int)desc.bNumConfigurations << endl;
  cout << "Device Class: "                      << (int)desc.bDeviceClass       << endl;
  cout << "VendorID:  0x" << setfill('0') << setw(4) << hex << desc.idVendor  << endl;
  cout << "ProductID: 0x" << setfill('0') << setw(4) << hex << desc.idProduct << endl;
  libusb_config_descriptor *config;
  libusb_get_config_descriptor(dev, 0, &config);
  cout << "Interfaces: " << (int)config->bNumInterfaces << endl;
  const libusb_interface *inter;
  const libusb_interface_descriptor *interdesc;
  const libusb_endpoint_descriptor *epdesc;
  for(int i=0; i<(int)config->bNumInterfaces; i++) {
    inter = &config->interface[i];
    cout << "  Interface " << i << endl;
    cout << "    Number of alternate settings: " << inter->num_altsetting << endl;
    for(int j=0; j<inter->num_altsetting; j++) {
      interdesc = &inter->altsetting[j];
      cout << "    Interface Number: " << (int)interdesc->bInterfaceNumber << endl;
      cout << "    Number of endpoints: " << (int)interdesc->bNumEndpoints << endl;
      for(int k=0; k<(int)interdesc->bNumEndpoints; k++) {
        epdesc = &interdesc->endpoint[k];
        cout << "      Descriptor Type: " << (int)epdesc->bDescriptorType << endl;
        cout << "      EP Address: " << (int)epdesc->bEndpointAddress << endl;
      }
    }
  }
  cout << endl << endl << endl;
  libusb_free_config_descriptor(config);
}

void
bulk_transfer_callback(libusb_transfer *tx)
{
  cerr << "bulk transfer" << endl;
  string s;
  for (int i = 0; i < tx->actual_length; ++i)
  {
    s += tx->buffer[i];
  }
  cerr << "Received: " << s << endl;
}

void
control_transfer_callback(USBControlTransfer &tx)
{
}

class KSpecControlTransfer : public USBControlTransfer
{
public:
  KSpecControlTransfer(USBDevice::Ptr device)
  : USBControlTransfer(device, 0x83, 64)
  {}

  virtual void on_complete()
  {
    cerr << "Completed. Received " << _buffer.size() << " bytes. ";
    cerr << endl;
    string s;
    int i = 0;
    foreach(char c, _buffer)
    {
      cerr << dec << setw(2) << setfill('0') << i++
           << ": 0x" << hex << setw(2) << setfill('0') << (int)(c)
           << "  '" << c << "'" << endl;
      s += c;
    }
    cerr << "\tData: " << s << endl;
  }
};

void
program2(po::variables_map &options, hh::Keyboard::Ptr &kb)
{
  cout << "Programming" << endl;
  USB usb(USB::DEBUG_LEVEL_3);
  USBDevice::Ptr keyboard;
  {
    USBDevice::List devices = usb.get_device_list();
    foreach(USBDevice::Ptr &device, *devices)
    {
      libusb_device_descriptor device_desc;
      device->get_device_descriptor(&device_desc);
      if (device_desc.idVendor == 0x05AF && device_desc.idProduct == 0x2032)
      {
        keyboard = device;
        break;
      }
    }
  }
  printdev(keyboard->ptr());
  keyboard->open();
  keyboard->claim_interface(2);

  KSpecControlTransfer control_transfer(keyboard);

  control_transfer.submit();

  usb.handle_events();
}

typedef boost::shared_ptr<libusb_context>        ContextPtr;
typedef boost::shared_ptr<libusb_device>         DevicePtr;
typedef boost::shared_array<libusb_device*>      DeviceArray;
typedef boost::shared_ptr<libusb_device_handle>  DeviceHandle;
typedef boost::shared_ptr<libusb_transfer>       TransferPtr;
typedef boost::shared_ptr<void>                  Interface;

void control_transfer_callback(libusb_transfer *tx)
{
  switch (tx->status)
  {
    case LIBUSB_TRANSFER_COMPLETED:
      {
        cerr << "Completed. Received " << tx->actual_length << " bytes. ";
        cerr << endl;
        string s;
        char c;
        for (int i = 0; i < tx->actual_length; ++i)
        {
          c = tx->buffer[i];
          cerr << dec << setw(2) << setfill('0') << i
               << ": 0x" << hex << setw(2) << setfill('0') << (int)(c)
               << "  '" << c << "'" << endl;
          s += c;
        }
        cerr << "\tData: " << s << endl;
      }
      break;
    case LIBUSB_TRANSFER_CANCELLED:
      std::cerr << "Cancelled." << std::endl;
      break;
    case LIBUSB_TRANSFER_ERROR:
      std::cerr << "Error."     << std::endl;
      break;
    case LIBUSB_TRANSFER_NO_DEVICE:
      std::cerr << "No device." << std::endl;
      break;
    case LIBUSB_TRANSFER_OVERFLOW:
      std::cerr << "Overflow."  << std::endl;
      break;
    case LIBUSB_TRANSFER_STALL:
      std::cerr << "Stalled."   << std::endl;
      break;
    case LIBUSB_TRANSFER_TIMED_OUT:
      std::cerr << "Timed out." << std::endl;
      break;
  }
}

void release_interface(libusb_device_handle *handle, void* iface)
{
  libusb_release_interface(handle, reinterpret_cast<ptrdiff_t>(iface));
}

void null_unref_device(libusb_device *dev) {}


ContextPtr usb_initialize()
{
  libusb_context *temp_context = NULL;
  int result = libusb_init(&temp_context);
  if (result != LIBUSB_SUCCESS) throw USBException(result);
  ContextPtr context(temp_context, libusb_exit);
  libusb_set_debug(context.get(), 3);
  return context;
}

ssize_t usb_get_devices(ContextPtr &context, DeviceArray &devices)
{
  libusb_device **temp_devices;
  ssize_t count = libusb_get_device_list(context.get(), &temp_devices);
  if (count < 0) throw USBException(count);
  devices.reset(temp_devices,
                boost::function<void(libusb_device**)>(boost::bind(&libusb_free_device_list, _1, 1)));
  return count;
}

void usb_get_device_descriptor(libusb_device *device, libusb_device_descriptor &device_desc)
{
  int result = libusb_get_device_descriptor(device, &device_desc);
  if (result != LIBUSB_SUCCESS) throw USBException(result);
}

DeviceHandle usb_open_device(DevicePtr &keyboard)
{
  libusb_device_handle *temp_handle;
  int result = libusb_open(keyboard.get(), &temp_handle);
  if (result != LIBUSB_SUCCESS) throw USBException(result);
  return DeviceHandle(temp_handle, libusb_close);
}

Interface usb_claim_interface(DeviceHandle &handle)
{
  int result = libusb_claim_interface(handle.get(), 2);
  if (result != LIBUSB_SUCCESS) throw USBException(result);
  return Interface(reinterpret_cast<void*>(2),
                   boost::bind(release_interface, handle.get(), _1));
}

TransferPtr usb_create_transfer(DeviceHandle &handle, vector<unsigned char> &buffer)
{
  TransferPtr transfer(libusb_alloc_transfer(0), libusb_free_transfer);
  if (!transfer) throw USBException(LIBUSB_ERROR_NO_MEM);

  libusb_fill_control_setup(&buffer[0],
                            LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_INTERFACE,
                            0, /* TODO: Constants for kspec request types */
                            0, 0, buffer.size());

  libusb_fill_control_transfer(transfer.get(), handle.get(), &buffer[0],
                               control_transfer_callback, NULL, 0);

  return transfer;
}

void usb_submit_transfer(TransferPtr &transfer)
{
  int result = libusb_submit_transfer(transfer.get());
  if (result != LIBUSB_SUCCESS) throw USBException(result);

  // Handle events
}

DevicePtr usb_get_specific_device(ContextPtr context, int vendor_id, int product_id)
{
  DeviceArray devices;
  ssize_t count = usb_get_devices(context, devices);
  while (count--)
  {
    libusb_device_descriptor device_desc;
    usb_get_device_descriptor(devices[count], device_desc);
    if (device_desc.idVendor == vendor_id && device_desc.idProduct == product_id)
    {
      return DevicePtr(libusb_ref_device(devices[count]), libusb_unref_device);
    }
  }
  throw USBException(LIBUSB_ERROR_NO_DEVICE);
}

void
program(po::variables_map &options, hh::Keyboard::Ptr &kb)
{
  ContextPtr context = usb_initialize();
  DevicePtr keyboard = usb_get_specific_device(context, 0x05AF, 0x2032);

  printdev(keyboard.get());

  vector<unsigned char> buffer(64);
  DeviceHandle handle    = usb_open_device(keyboard);
  Interface    interface = usb_claim_interface(handle);
  TransferPtr  transfer  = usb_create_transfer(handle, buffer);

  usb_submit_transfer(transfer);
  libusb_handle_events(context.get());
}
