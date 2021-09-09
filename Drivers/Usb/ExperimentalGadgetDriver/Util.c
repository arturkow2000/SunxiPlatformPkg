#include "Driver.h"

EFI_STATUS UsbGadgetEp0Queue(GADGET_DRIVER_INTERNAL *Internal, VOID *Buffer, UINT32 Length, USB_PPI_REQ_COMPLETE_CALLBACK Callback, UINT32 Flags) {
  EFI_STATUS Status;

  Status = Internal->Usb->InitRequest(
    Internal->Usb,
    Internal->ControlRequest,
    Buffer,
    Length,
    Flags,
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

EFI_STATUS UsbGadgetEp0Respond(GADGET_DRIVER_INTERNAL *Internal, USB_DEVICE_REQUEST *Request, VOID *Buffer, UINT32 Length) {
  EFI_STATUS Status;
  UINT32 n = MIN(Request->Length, Length);

  DEBUG((EFI_D_INFO, "ep0 resp len %d requested %d total %d zlp %d\n", n, Request->Length, Length, Length < Request->Length));

  Status = Internal->Usb->InitRequest(
    Internal->Usb,
    Internal->ControlRequest,
    Buffer,
    n,
    Length < Request->Length ? USB_PPI_FLAG_ZLP : 0,
    NULL,
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
