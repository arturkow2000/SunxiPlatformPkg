#include "Driver.h"

STATIC EFI_STATUS UsbGadgetReset(GADGET_DRIVER_INTERNAL *Driver) {
  Driver->CdcState.LineCoding.RteRate = 115200;
  Driver->CdcState.LineCoding.ParityType = USB_CDC_NO_PARITY;
  Driver->CdcState.LineCoding.CharFormat = USB_CDC_1_STOP_BITS;
  Driver->CdcState.LineCoding.DataBits = 8;

  return EFI_SUCCESS;
}

STATIC EFI_STATUS UsbGadgetInitializeEndpoints(
  GADGET_DRIVER_INTERNAL *Driver,
  UINT32 NumberOfEndpoints
) {
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;

  Config->CustomEpIn.EndpointAddress = 1 | USB_ENDPOINT_DIR_IN;
  Config->CustomEpOut.EndpointAddress = 1;

  Config->CdcDataEpIn.EndpointAddress = 2 | USB_ENDPOINT_DIR_IN;
  Config->CdcDataEpOut.EndpointAddress = 2;

  Config->CdcControlEp.EndpointAddress = 3 | USB_ENDPOINT_DIR_IN;

  return EFI_SUCCESS;
}

STATIC EFI_STATUS UsbGadgetInitialize(GADGET_DRIVER_INTERNAL *Driver) {
  EFI_STATUS Status;
  UINT32 NumberOfEndpoints;

  Status = Driver->Usb->AllocateUrbs(Driver->Usb, 1, &Driver->ControlRequest);
  if (EFI_ERROR(Status))
    return Status;

  Status = Driver->Usb->AllocateUrbs(Driver->Usb, 1, &Driver->CdcDataInUrb);
  if (EFI_ERROR(Status))
    return Status;

  Status = Driver->Usb->AllocateUrbs(Driver->Usb, 1, &Driver->CdcDataOutUrb);
  if (EFI_ERROR(Status))
    return Status;

  Driver->CdcBuffer = AllocatePool(CDC_DATA_MAX_PACKET);
  if (!Driver)
    return EFI_OUT_OF_RESOURCES;

  Driver->ConfigDescriptor = AllocateCopyPool(sizeof gConfigDescriptorTemplate, &gConfigDescriptorTemplate);
  if (!Driver->ConfigDescriptor)
    return EFI_OUT_OF_RESOURCES;

  Status = Driver->Usb->GetNumberOfEndpoints(Driver->Usb, &NumberOfEndpoints);
  if (EFI_ERROR(Status))
    return Status;

  // Iterate over every endpoint descriptor, find proper hardware endpoint and update descriptor
  Status = UsbGadgetInitializeEndpoints(Driver, NumberOfEndpoints);
  if (EFI_ERROR(Status))
    return Status;

  Status = UsbGadgetReset(Driver);
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

  ZeroMem(Driver, sizeof(GADGET_DRIVER_INTERNAL));

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