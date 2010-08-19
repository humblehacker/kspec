#include <iostream>
#include <iomanip>
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
program(po::variables_map &options, hh::Keyboard::Ptr &kb)
{
  cout << "Programming" << endl;
  USB usb(3);
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

  // Async I/O

  // 1. Allocation
  // 2. Filling
  USBBulkTransfer transfer(keyboard, 0x83, 64, bulk_transfer_callback);

  // 3. Submission
  transfer.submit();

  // 4. Completion handling
  usb.handle_events();

  // 5. Deallocation
  //    automatic
}


