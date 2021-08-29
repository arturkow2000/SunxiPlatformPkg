#include "Driver.h"

STATIC EFI_STATUS UsbGadgetInitialize(GADGET_DRIVER_INTERNAL *Driver) {
  EFI_STATUS Status;

  Status = Driver->Usb->AllocateRequests(Driver->Usb, 1, &Driver->ControlRequest);
  if (EFI_ERROR(Status))
    return Status;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GadgetInitialize(
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status;
  USB_PPI *Usb;
  GADGET_DRIVER_INTERNAL *Driver;

  Status = PeiServicesLocatePpi(&gUsbPpiGuid, 0, NULL, (VOID**)&Usb);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = PeiServicesAllocatePool(sizeof(GADGET_DRIVER_INTERNAL), (VOID**)&Driver);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Driver->Public.HandleControlRequest = UsbGadgetHandleControlRequest;
  Driver->Usb = Usb;

  Status = UsbGadgetInitialize(Driver);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = Usb->RegisterGadgetDriver(Usb, &Driver->Public, NULL);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  DEBUG((EFI_D_ERROR, "Waiting for interrupts\n"));
  while (TRUE) {
    Usb->HandleInterrupts(Usb);
  }

  return EFI_SUCCESS;
}