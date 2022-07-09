#include "Driver.h"

INT8 UsbForwardControlRequestToGadgetDriver(USB_DRIVER *Driver, USB_DEVICE_REQUEST *Request) {
  EFI_STATUS Status;
  USB_DXE_DRIVER *DxeDriver = USB_DRIVER_INTO_DXE_DRIVER(Driver);

  ASSERT(DxeDriver->GadgetDriver);
  ASSERT(DxeDriver->GadgetDriver->HandleControlRequest);

  Status = DxeDriver->GadgetDriver->HandleControlRequest(DxeDriver->GadgetDriver, Request);
  if (Status == EFI_SUCCESS)
    return 1;
  else if (Status == EFI_UNSUPPORTED)
    return 0;
  else return -1;
}

VOID UsbSignalCompletion(USB_DRIVER *Driver, UINT32 Endpoint, USB_REQUEST_BLOCK *Urb, EFI_STATUS Status) {
  USB_DXE_DRIVER *DxeDriver = USB_DRIVER_INTO_DXE_DRIVER(Driver);

  DEBUG((EFI_D_INFO, "URB %p len %d complete: %r\n", Urb->Buffer, Urb->Actual, Status));
  ASSERT(DxeDriver->GadgetDriver);
  Urb->Status = Status;
  if (Urb->Callback) {
    Urb->Callback((USB_GADGET*)DxeDriver->GadgetDriver, Urb);
  }
}

VOID UsbSignalReset(USB_DRIVER *Driver) {
  USB_DXE_DRIVER *DxeDriver = USB_DRIVER_INTO_DXE_DRIVER(Driver);
  ASSERT(DxeDriver->GadgetDriver);
  ASSERT(DxeDriver->GadgetDriver->HandleReset);
  DxeDriver->GadgetDriver->HandleReset(DxeDriver->GadgetDriver);
}
