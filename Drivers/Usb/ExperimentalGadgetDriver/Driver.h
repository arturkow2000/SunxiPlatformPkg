#pragma once

#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>

#include <Library/UsbGadgetLib.h>
#include <Ppi/UsbPpi.h>

#include <IndustryStandard/UsbEx.h>

#define GADGET_DRIVER_GET_INTERNAL(Record)        \
  BASE_CR(Record, GADGET_DRIVER_INTERNAL, Public)

typedef struct _USB_STRING_TABLE {
  UINT8 Id;
  CONST CHAR16 *String;
} USB_STRING_TABLE;

typedef struct _GADGET_DRIVER_INTERNAL {
  USB_GADGET_DRIVER Public;
  USB_PPI *Usb;

  USB_REQUEST *ControlRequest;
  INT8 ControlRequestStatus;
} GADGET_DRIVER_INTERNAL;

EFI_STATUS UsbGadgetHandleControlRequest(
  USB_GADGET_DRIVER *This,
  USB_DRIVER *Driver,
  USB_DEVICE_REQUEST *Request
  );
  