#pragma once

#include <PiDxe.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>

#include "../Common/Driver.h"

#include <Protocol/UsbProtocol.h>

#define GET_DXE_DRIVER(Record)  \
  BASE_CR(Record, USB_DXE_DRIVER, Proto)

#define USB_DRIVER_INTO_DXE_DRIVER(Record)  \
  BASE_CR(Record, USB_DXE_DRIVER, Common)

typedef struct _USB_DXE_DRIVER {
  USB_PROTOCOL Proto;
  USB_DRIVER Common;

  USB_GADGET_INTERFACE *GadgetDriver;
  EFI_EVENT Timer;
} USB_DXE_DRIVER;
