#include "Driver.h"
#include <IndustryStandard/CdcAcm.h>

//STATIC EFI_STATUS UsbCdcDumpLineCoding(USB_CDC_LINE_CODING )

STATIC USB_CDC_LINE_CODING mLineCoding;

STATIC VOID SetLineCoding(
  USB_PPI *Usb,
  UINT32 Endpoint,
  VOID *Buffer,
  UINT32 Length,
  GADGET_DRIVER_INTERNAL *UserData,
  EFI_STATUS Status
  )
{
  DEBUG((EFI_D_INFO, "SET_LINE_CODING %p %d %d\n", Buffer, Length, sizeof(mLineCoding)));
  DEBUG((EFI_D_INFO, "Status = %r\n", Status));

  DEBUG((EFI_D_INFO, "Rate: %d\n", mLineCoding.RteRate));
  switch(mLineCoding.ParityType) {
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
  switch(mLineCoding.CharFormat) {
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
  DEBUG((EFI_D_INFO, "Data bits: %d\n", mLineCoding.DataBits));
}

EFI_STATUS UsbGadgetHandleCdcRequest(GADGET_DRIVER_INTERNAL *Internal, USB_DEVICE_REQUEST *Request) {
  EFI_STATUS Status;

  switch (Request->Request)
  {
  case USB_CDC_REQ_SET_LINE_CODING:
    DEBUG((EFI_D_INFO, "Reading line coding\n"));
    Status = UsbGadgetEp0Queue(Internal, &mLineCoding, sizeof(mLineCoding), (USB_PPI_REQ_COMPLETE_CALLBACK)SetLineCoding);
    ASSERT_EFI_ERROR(Status);

    return EFI_SUCCESS;
    break;
  
  default:
    DEBUG((EFI_D_ERROR, "Unknown CDC request %d\n", Request->Request));
    return EFI_UNSUPPORTED;
  }
}