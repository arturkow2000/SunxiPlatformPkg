#pragma once

#include <IndustryStandard/Usb.h>

typedef struct _USB_DRIVER USB_DRIVER;
typedef struct _USB_GADGET_INTERFACE USB_GADGET_INTERFACE;
typedef struct _USB_GADGET USB_GADGET;
typedef struct _USB_REQUEST_BLOCK USB_REQUEST_BLOCK;

typedef
VOID
(EFIAPI *USB_GADGET_REQUEST_COMPLETION_HANDLER)(
  USB_GADGET *This,
  USB_REQUEST_BLOCK *Urb
);

typedef
EFI_STATUS
(EFIAPI *USB_GADGET_HANDLE_CONTROL_REQUEST)(
  USB_GADGET_INTERFACE *This,
  USB_DEVICE_REQUEST *Request
);

typedef USB_DEVICE_DESCRIPTOR*
(EFIAPI *USB_GADGET_GET_DEVICE_DESCRIPTOR)(
  USB_GADGET *This
);

typedef USB_CONFIG_DESCRIPTOR*
(EFIAPI *USB_GADGET_GET_CONFIG_DESCRIPTOR)(
  USB_GADGET *This
);

typedef UINT8*
(EFIAPI *USB_GADGET_GET_STRING_DESCRIPTOR)(
  USB_GADGET *This,
  UINT8 Id
);

typedef EFI_STATUS
(EFIAPI *USB_GADGET_HANDLE_CLASS_REQUEST)(
  USB_GADGET *This,
  USB_DEVICE_REQUEST *Request
);

typedef EFI_STATUS
(EFIAPI *USB_GADGET_HANDLE_VENDOR_REQUEST)(
  USB_GADGET *This,
  USB_DEVICE_REQUEST *Request
);

typedef EFI_STATUS
(EFIAPI *USB_GADGET_HANDLE_SET_CONFIG)(
  USB_GADGET *This,
  UINT8 Config
);

// Low-level interface to USB driver
struct _USB_GADGET_INTERFACE {
  USB_GADGET_HANDLE_CONTROL_REQUEST HandleControlRequest;
};

#define USB_URB_FROM_LINK(Record)        \
  BASE_CR(Record, USB_REQUEST_BLOCK, Node)

struct _USB_REQUEST_BLOCK {
  LIST_ENTRY Node;

  EFI_STATUS Status;

  VOID *Buffer;
  UINT32 Length;

  // Number of bytes already transferred
  UINT32 Actual;
  UINT32 Flags;

  USB_GADGET_REQUEST_COMPLETION_HANDLER Callback;
};

struct _USB_GADGET {
  USB_GADGET_INTERFACE Interface;
  USB_REQUEST_BLOCK *ControlUrb;

  USB_GADGET_GET_DEVICE_DESCRIPTOR GetDeviceDescriptor;
  USB_GADGET_GET_CONFIG_DESCRIPTOR GetConfigDescriptor;
  USB_GADGET_GET_STRING_DESCRIPTOR GetStringDescriptor;
  USB_GADGET_HANDLE_CLASS_REQUEST HandleClassRequest;
  USB_GADGET_HANDLE_VENDOR_REQUEST HandleVendorRequest;
  USB_GADGET_HANDLE_SET_CONFIG HandleSetConfig;
};

#define USB_FLAG_ZLP (1 << 0)
#define USB_FLAG_TX (1 << 1)

EFI_STATUS UsbGadgetInitializeInterface(USB_GADGET *);
EFI_STATUS UsbGadgetDriverRegister(USB_GADGET *);
EFI_STATUS UsbGadgetDriverUnregister(USB_GADGET *);
USB_REQUEST_BLOCK *UsbGadgetAllocateUrb(VOID);
EFI_STATUS UsbGadgetInitUrb(
  USB_GADGET *Gadget,
  USB_REQUEST_BLOCK *Urb,
  VOID *Buffer,
  UINT32 Length,
  UINT32 Flags,
  USB_GADGET_REQUEST_COMPLETION_HANDLER Callback
);
EFI_STATUS UsbGadgetQueueUrb(
  USB_GADGET *Gadget,
  UINT32 Endpoint,
  USB_REQUEST_BLOCK *Urb
);
EFI_STATUS UsbGadgetQueue(
  USB_GADGET *Gadget,
  UINT32 Endpoint,
  USB_REQUEST_BLOCK *Urb,
  VOID *Buffer,
  UINT32 Length,
  UINT32 Flags,
  USB_GADGET_REQUEST_COMPLETION_HANDLER Callback
);
EFI_STATUS UsbGadgetEnableEndpoint(
  USB_GADGET *Gadget,
  USB_ENDPOINT_DESCRIPTOR *Endpoint
);
EFI_STATUS UsbGadgetHaltEndpoint(USB_GADGET *Gadget, UINT32 Endpoint);
