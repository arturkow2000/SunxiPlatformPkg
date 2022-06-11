#pragma once

#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Library/UsbGadgetLib.h>
#include <Ppi/UsbPpi.h>

#include <IndustryStandard/UsbEx.h>
#include <IndustryStandard/CdcAcm.h>

#define CUSTOM_INTERFACE 0
#define CDC_CONTROL_INTERFACE 1
#define CDC_DATA_INTEFACE 2

#if FixedPcdGet32(Usb20Support)
#define CDC_DATA_MAX_PACKET 512
#else
#define CDC_DATA_MAX_PACKET 64
#endif

#define GADGET_TO_DRIVER(Record)        \
  BASE_CR(Record, GADGET_DRIVER, Gadget)

typedef struct {
  USB_CDC_LINE_CODING PendingLineCoding;
  USB_CDC_LINE_CODING LineCoding;
} CDC_STATE;

typedef struct _GADGET_DRIVER {
  USB_GADGET Gadget;

  USB_REQUEST_BLOCK *CdcDataInUrb;
  USB_REQUEST_BLOCK *CdcDataOutUrb;

  UINT8 *CdcBuffer;

  CDC_STATE CdcState;
  USB_CONFIG_DESCRIPTOR *ConfigDescriptor;
} GADGET_DRIVER;

typedef struct _DEVICE_CONFIG {
  USB_CONFIG_DESCRIPTOR Config;
  // Interface with custom protocol used for controlling device
  // currently dummy,
  // it will be used to transfer EFI image when booting from USB
  // and possibly for debugging purposes
  //
  // CDC will be used to emulate serial port
  USB_INTERFACE_DESCRIPTOR Custom;
  USB_ENDPOINT_DESCRIPTOR CustomEpIn;
  USB_ENDPOINT_DESCRIPTOR CustomEpOut;
  USB_INTERFACE_ASSOCIATION_DESCRIPTOR Iad0;
  USB_INTERFACE_DESCRIPTOR CdcAt;
  USB_CDC_FUNCTIONAL_DESCRIPTOR CdcFunctional;
  USB_CDC_CALL_MANAGEMENT_DESCRIPTOR CdcCallManagement;
  USB_CDC_ACM_DESCRIPTOR CdcAcm;
  USB_CDC_UNION_DESCRIPTOR CdcUnion;
  USB_ENDPOINT_DESCRIPTOR CdcControlEp;
  USB_INTERFACE_DESCRIPTOR CdcData;
  USB_ENDPOINT_DESCRIPTOR CdcDataEpIn;
  USB_ENDPOINT_DESCRIPTOR CdcDataEpOut;
} DEVICE_CONFIG;

extern DEVICE_CONFIG gConfigDescriptorTemplate;

USB_DEVICE_DESCRIPTOR *UsbGadgetGetDeviceDescriptor(USB_GADGET *Gadget);
USB_CONFIG_DESCRIPTOR *UsbGadgetGetConfigDescriptor(USB_GADGET *Gadget);
UINT8 *UsbGadgetGetStringDescriptor(USB_GADGET *Gadget, UINT8 Id);
EFI_STATUS UsbGadgetHandleClassRequest(USB_GADGET *This, USB_DEVICE_REQUEST *Request);
EFI_STATUS UsbGadgetHandleVendorRequest(USB_GADGET *This, USB_DEVICE_REQUEST *Request);
EFI_STATUS UsbGadgetHandleSetConfig(USB_GADGET *This, UINT8 Config);

EFI_STATUS CdcHandleRequest(USB_GADGET *This, USB_DEVICE_REQUEST *Request);
EFI_STATUS CdcEnable(USB_GADGET *This);

#if 0
/**
 Responds to USB IN request
**/
EFI_STATUS UsbGadgetEp0Respond(GADGET_DRIVER_INTERNAL *Internal, USB_DEVICE_REQUEST *Request, VOID *Buffer, UINT32 Length);
EFI_STATUS UsbGadgetEp0Queue(GADGET_DRIVER_INTERNAL *Internal, VOID *Buffer, UINT32 Length, USB_PPI_REQ_COMPLETE_CALLBACK Callback, UINT32 Flags);

EFI_STATUS UsbGadgetEpxInitAndQueue(
  GADGET_DRIVER_INTERNAL *Internal,
  USB_REQUEST_BLOCK *Urb,
  USB_ENDPOINT_DESCRIPTOR *Endpoint,
  VOID *Buffer,
  UINT32 Length,
  UINT32 Flags,
  USB_PPI_REQ_COMPLETE_CALLBACK Callback
);


// CDC API

EFI_STATUS CdcEnable(GADGET_DRIVER_INTERNAL *Internal);
EFI_STATUS CdcHandleRequest(GADGET_DRIVER_INTERNAL *Internal, USB_DEVICE_REQUEST *Request);
#endif
