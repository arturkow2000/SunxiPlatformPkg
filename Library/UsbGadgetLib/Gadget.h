#pragma once

#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UsbGadgetLib.h>

#include <IndustryStandard/Usb.h>
#include <IndustryStandard/UsbEx.h>
#include <Protocol/UsbProtocol.h>

#define INTERFACE_TO_GADGET(Record)        \
  BASE_CR(Record, USB_GADGET, Interface)

EFI_STATUS UsbGadgetHandleControlRequest(
  USB_GADGET_INTERFACE *This,
  USB_DEVICE_REQUEST *Request
);
