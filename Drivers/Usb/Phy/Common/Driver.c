#include "Driver.h"

EFI_STATUS UsbPhyInit(USB_PHY_DRIVER *Driver) {
  return UsbPhyInitGpio(Driver);
}