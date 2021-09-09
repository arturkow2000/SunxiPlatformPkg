#pragma once

#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>

#include <Library/UsbGadgetLib.h>
#include <Ppi/UsbPpi.h>

#include <IndustryStandard/UsbEx.h>
#include <IndustryStandard/CdcAcm.h>

#define CUSTOM_INTERFACE 0
#define CDC_CONTROL_INTERFACE 1
#define CDC_DATA_INTEFACE 2

#define GADGET_DRIVER_GET_INTERNAL(Record)        \
  BASE_CR(Record, GADGET_DRIVER_INTERNAL, Public)

typedef struct {
  USB_CDC_LINE_CODING LineCoding;
} CDC_STATE;

typedef struct _GADGET_DRIVER_INTERNAL {
  USB_GADGET_DRIVER Public;
  USB_PPI *Usb;

  USB_REQUEST *ControlRequest;

  CDC_STATE CdcState;
} GADGET_DRIVER_INTERNAL;

EFI_STATUS UsbGadgetHandleControlRequest(
  USB_GADGET_DRIVER *This,
  USB_DRIVER *Driver,
  USB_DEVICE_REQUEST *Request
  );

EFI_STATUS UsbGadgetHandleCdcRequest(GADGET_DRIVER_INTERNAL *Internal, USB_DEVICE_REQUEST *Request);

/**
 Responds to USB IN request
**/
EFI_STATUS UsbGadgetEp0Respond(GADGET_DRIVER_INTERNAL *Internal, USB_DEVICE_REQUEST *Request, VOID *Buffer, UINT32 Length);

EFI_STATUS UsbGadgetEp0Queue(GADGET_DRIVER_INTERNAL *Internal, VOID *Buffer, UINT32 Length, USB_PPI_REQ_COMPLETE_CALLBACK Callback, UINT32 Flags);
