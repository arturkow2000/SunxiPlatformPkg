#include "Driver.h"

INT8 UsbForwardControlRequestToGadgetDriver(USB_DRIVER *Driver, USB_DEVICE_REQUEST *Request) {
  EFI_STATUS Status;
  USB_PEI_DRIVER *PeiDriver = USB_DRIVER_INTO_PEI_DRIVER(Driver);

  ASSERT(PeiDriver->GadgetDriver);
  ASSERT(PeiDriver->GadgetDriver->HandleControlRequest);

  Status = PeiDriver->GadgetDriver->HandleControlRequest(PeiDriver->GadgetDriver, (USB_DRIVER*)&PeiDriver->Ppi, Request);
  if (Status == EFI_SUCCESS)
    return 1;
  else if (Status == EFI_UNSUPPORTED)
    return 0;
  else return -1;
}

VOID UsbSignalCompletion(USB_DRIVER *Driver, UINT32 Endpoint, USB_REQUEST_BLOCK *Urb, EFI_STATUS Status) {
  USB_PEI_DRIVER *PeiDriver = USB_DRIVER_INTO_PEI_DRIVER(Driver);
  USB_PPI_REQ_COMPLETE_CALLBACK Callback;

  if (Urb->Callback) {
    Callback = Urb->Callback;

    Callback(&PeiDriver->Ppi, Endpoint, Urb->Buffer, Urb->Actual, Urb->UserData, Status);
  }
}
