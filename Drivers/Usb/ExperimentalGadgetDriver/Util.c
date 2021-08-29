#include "Driver.h"

EFI_STATUS UsbGadgetEp0Queue(GADGET_DRIVER_INTERNAL *Internal, VOID *Buffer, UINT32 Length, USB_PPI_REQ_COMPLETE_CALLBACK Callback) {
  EFI_STATUS Status;

  Status = Internal->Usb->InitRequest(
    Internal->Usb,
    Internal->ControlRequest,
    Buffer,
    Length,
    0,
    Callback,
    Internal
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "InitRequest() failed: %r\n", Status));
    return Status;
  }
    
  Status = Internal->Usb->Ep0Queue(
    Internal->Usb,
    Internal->ControlRequest
  );
  if (EFI_ERROR(Status))
    DEBUG((EFI_D_ERROR, "QueueEp0Packet() failed: %r\n", Status));

  return Status;
}