#include "Driver.h"

STATIC VOID SetLineCoding(
  USB_PPI *Usb,
  UINT32 Endpoint,
  VOID *Buffer,
  UINT32 Length,
  GADGET_DRIVER_INTERNAL *Internal,
  EFI_STATUS Status
  )
{
  USB_CDC_LINE_CODING *LineCoding = Buffer;

  if (EFI_ERROR(Status))
    return;

  if (Length != sizeof(USB_CDC_LINE_CODING)) {
    Internal->Usb->Halt(Internal->Usb, 0);
    return;
  }

  DEBUG((EFI_D_INFO, "Rate: %d\n", LineCoding->RteRate));
  switch(LineCoding->ParityType) {
  case USB_CDC_NO_PARITY:
    DEBUG((EFI_D_INFO, "Parity: None\n"));
    break;
  case USB_CDC_ODD_PARITY:
    DEBUG((EFI_D_INFO, "Parity: Odd\n"));
    break;
  case USB_CDC_EVEN_PARITY:
    DEBUG((EFI_D_INFO, "Parity: Even\n"));
    break;
  case USB_CDC_MARK_PARITY:
    DEBUG((EFI_D_INFO, "Parity: Mark\n"));
    break;
  case USB_CDC_SPACE_PARITY:
    DEBUG((EFI_D_INFO, "Parity: Space\n"));
    break;
  default:
    DEBUG((EFI_D_INFO, "Parity: Unknown\n"));
    break;
  }
  switch(LineCoding->CharFormat) {
  case USB_CDC_1_STOP_BITS:
    DEBUG((EFI_D_INFO, "Stop bits: 1\n"));
    break;
  case USB_CDC_1_5_STOP_BITS:
    DEBUG((EFI_D_INFO, "Stop bits: 1.5\n"));
    break;
  case USB_CDC_2_STOP_BITS:
    DEBUG((EFI_D_INFO, "Stop bits: 2\n"));
    break;
  default:
    DEBUG((EFI_D_INFO, "Stop bits: unknown\n"));
    break;
  }
  DEBUG((EFI_D_INFO, "Data bits: %d\n", LineCoding->DataBits));
}

EFI_STATUS UsbGadgetHandleCdcRequest(GADGET_DRIVER_INTERNAL *Internal, USB_DEVICE_REQUEST *Request) {
  EFI_STATUS Status;

  // Match direction, target and request
  // Request type was already handled by caller
  switch (((Request->RequestType & (USB_ENDPOINT_DIR_IN | 3)) << 8) | Request->Request)
  {
  case (USB_TARGET_INTERFACE << 8) | USB_CDC_REQ_SET_LINE_CODING:
    return UsbGadgetEp0Queue(Internal, &Internal->CdcState.LineCoding, sizeof(USB_CDC_LINE_CODING), (USB_PPI_REQ_COMPLETE_CALLBACK)SetLineCoding, 0);

  case ((USB_ENDPOINT_DIR_IN | USB_TARGET_INTERFACE) << 8) | USB_CDC_REQ_GET_LINE_CODING:
    DEBUG((EFI_D_INFO, "============================ Sending line coding\n"));
    Status = UsbGadgetEp0Queue(
      Internal,
      &Internal->CdcState.LineCoding,
      sizeof(Internal->CdcState.LineCoding),
      NULL,
      0
    );

    ASSERT_EFI_ERROR(Status);
    return Status;

  case (USB_TARGET_INTERFACE << 8) | USB_CDC_REQ_SET_CONTROL_LINE_STATE:
    return UsbGadgetEp0Queue(Internal, NULL, 0, NULL, 0);
  
  default:
    DEBUG((EFI_D_ERROR, "Unknown CDC request %d dir %s\n", Request->Request, (Request->RequestType & USB_ENDPOINT_DIR_IN) ? L"IN" : L"OUT"));
    return EFI_UNSUPPORTED;
  }
}