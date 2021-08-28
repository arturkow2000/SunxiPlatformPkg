#pragma once

#include <IndustryStandard/Usb.h>

typedef struct _USB_DRIVER USB_DRIVER;
typedef struct _USB_GADGET_DRIVER USB_GADGET_DRIVER;

typedef
EFI_STATUS
(EFIAPI *USB_GADGET_HANDLE_CONTROL_REQUEST)(
  USB_GADGET_DRIVER *This,
  USB_DRIVER *Driver,
  USB_DEVICE_REQUEST *Request
);

struct _USB_GADGET_DRIVER {
  USB_GADGET_HANDLE_CONTROL_REQUEST HandleControlRequest;
};
