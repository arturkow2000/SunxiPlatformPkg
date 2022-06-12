#include "Driver.h"

STATIC EFI_STATUS UsbGadgetReset(GADGET_DRIVER *Driver) {
  Driver->CdcState.LineCoding.RteRate = 115200;
  Driver->CdcState.LineCoding.ParityType = USB_CDC_NO_PARITY;
  Driver->CdcState.LineCoding.CharFormat = USB_CDC_1_STOP_BITS;
  Driver->CdcState.LineCoding.DataBits = 8;

  return EFI_SUCCESS;
}

STATIC VOID UsbGadgetInitializeEndpoints(
  GADGET_DRIVER *Driver
) {
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;

  // FIXME: makes assumption on hardware, should detect hardware and dynamically
  // allocate proper endpoints.
  Config->CustomEpIn.EndpointAddress = 1 | USB_ENDPOINT_DIR_IN;
  Config->CustomEpOut.EndpointAddress = 1;

  Config->CdcDataEpIn.EndpointAddress = 2 | USB_ENDPOINT_DIR_IN;
  Config->CdcDataEpOut.EndpointAddress = 2;

  Config->CdcControlEp.EndpointAddress = 3 | USB_ENDPOINT_DIR_IN;
}

STATIC EFI_STATUS UsbGadgetInitialize(GADGET_DRIVER *Driver) {
  EFI_STATUS Status;

  Driver->CdcDataInUrb = UsbGadgetAllocateUrb();
  if (!Driver->CdcDataInUrb)
    return EFI_OUT_OF_RESOURCES;
  
  Driver->CdcDataOutUrb = UsbGadgetAllocateUrb();
  if (!Driver->CdcDataOutUrb)
    return EFI_OUT_OF_RESOURCES;

  Driver->CdcRxBuffer = AllocatePool(CDC_DATA_MAX_PACKET);
  if (!Driver->CdcRxBuffer)
    return EFI_OUT_OF_RESOURCES;

  Status = SimpleBufferInit(&Driver->CdcTxBuffer, CDC_DATA_MAX_PACKET);
  if (EFI_ERROR(Status))
    return Status;

  Driver->CdcRxBufferLength = 0;
  Driver->CdcRxBufferDataOffset = 0;
  Driver->SerialHandle = NULL;

  Driver->ConfigDescriptor = AllocateCopyPool(sizeof gConfigDescriptorTemplate, &gConfigDescriptorTemplate);
  if (!Driver->ConfigDescriptor)
    return EFI_OUT_OF_RESOURCES;

  UsbGadgetInitializeEndpoints(Driver);

  return UsbGadgetReset(Driver);
}

EFI_STATUS
EFIAPI
GadgetInitialize(
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  GADGET_DRIVER *Driver;

  Driver = AllocatePool(sizeof(GADGET_DRIVER));
  if (!Driver)
    return EFI_OUT_OF_RESOURCES;

  Status = UsbGadgetInitialize(Driver);
  if (EFI_ERROR(Status))
    return Status;

  UsbGadgetInitializeInterface(&Driver->Gadget);
  Driver->Gadget.GetDeviceDescriptor = UsbGadgetGetDeviceDescriptor;
  Driver->Gadget.GetConfigDescriptor = UsbGadgetGetConfigDescriptor;
  Driver->Gadget.GetStringDescriptor = UsbGadgetGetStringDescriptor;
  Driver->Gadget.HandleClassRequest = UsbGadgetHandleClassRequest;
  Driver->Gadget.HandleVendorRequest = UsbGadgetHandleVendorRequest;
  Driver->Gadget.HandleSetConfig = UsbGadgetHandleSetConfig;

  Status = UsbGadgetDriverRegister(&Driver->Gadget);
  if (EFI_ERROR(Status))
    return Status;

  return EFI_SUCCESS;
}
