#include "Gadget.h"

STATIC USB_PROTOCOL *mUsb;

STATIC USB_DEVICE_DESCRIPTOR *GetDeviceDescriptorNull(
  USB_GADGET *This
) {
  DEBUG((EFI_D_ERROR, "Gadget driver must implement GetDeviceDescriptor\n"));
  ASSERT(0);
  return NULL;
}

STATIC USB_CONFIG_DESCRIPTOR *GetConfigDescriptorNull(
  USB_GADGET *This
) {
  DEBUG((EFI_D_ERROR, "Gadget driver must implement GetConfigDescriptor\n"));
  ASSERT(0);
  return NULL;
}

STATIC UINT8 *GetStringDescriptorNull(
  USB_GADGET *This,
  UINT8 Id
) {
  return NULL;
}

STATIC USB_DEVICE_QUALIFIER_DESCRIPTOR *GetDeviceQualifierDescriptorNull(
  USB_GADGET *This
) {
  return NULL;
}

STATIC USB_BOS_DESCRIPTOR *GetBosDescriptorNull(
  USB_GADGET *This
) {
  return NULL;
}

STATIC EFI_STATUS HandleClassRequestNull(
  USB_GADGET *This,
  USB_DEVICE_REQUEST *Request
) {
  return EFI_UNSUPPORTED;
}

STATIC EFI_STATUS HandleVendorRequestNull(
  USB_GADGET *This,
  USB_DEVICE_REQUEST *Request
) {
  return EFI_UNSUPPORTED;
}

STATIC EFI_STATUS HandleSetConfigNull(
  USB_GADGET *This,
  UINT8 Config
) {
  DEBUG((EFI_D_ERROR, "Gadget driver must implement HandleSetConfig\n"));
  ASSERT(0);
  return EFI_UNSUPPORTED;
}

STATIC EFI_STATUS UsbGadgetHandleReset(USB_GADGET_INTERFACE *This) {
  USB_GADGET* Gadget = INTERFACE_TO_GADGET(This);
  EFI_STATUS Status;

  DEBUG((EFI_D_INFO, "USB reset, disabling gadget\n"));
  Status = Gadget->HandleSetConfig(Gadget, 0);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Could not disable USB gadget\n"));
  }

  return Status;
}

EFI_STATUS EFIAPI UsbGadgetLibConstructor(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
) {
  EFI_STATUS Status;

  Status = gBS->LocateProtocol(&gUsbProtocolGuid, NULL, (VOID**)&mUsb);
  ASSERT_EFI_ERROR(Status);

  return Status;
}

EFI_STATUS UsbGadgetInitializeInterface(USB_GADGET *Gadget) {
  ASSERT(Gadget);
  Gadget->Interface.HandleControlRequest = UsbGadgetHandleControlRequest;
  Gadget->Interface.HandleReset = UsbGadgetHandleReset;
  Gadget->GetDeviceDescriptor = GetDeviceDescriptorNull;
  Gadget->GetConfigDescriptor = GetConfigDescriptorNull;
  Gadget->GetStringDescriptor = GetStringDescriptorNull;
  Gadget->GetDeviceQualifierDescriptor = GetDeviceQualifierDescriptorNull;
  Gadget->GetBosDescriptor = GetBosDescriptorNull;
  Gadget->HandleClassRequest = HandleClassRequestNull;
  Gadget->HandleVendorRequest = HandleVendorRequestNull;
  Gadget->HandleSetConfig = HandleSetConfigNull;

  Gadget->ControlUrb = UsbGadgetAllocateUrb();
  if (!Gadget->ControlUrb)
    return EFI_OUT_OF_RESOURCES;

  return EFI_SUCCESS;
}

EFI_STATUS UsbGadgetDriverRegister(USB_GADGET *Gadget) {
  return mUsb->RegisterGadgetDriver(mUsb, &Gadget->Interface, Gadget);
}

EFI_STATUS UsbGadgetDriverUnregister(USB_GADGET *Gadget) {
  return mUsb->UnregisterGadgetDriver(mUsb, &Gadget->Interface);
}

USB_REQUEST_BLOCK *UsbGadgetAllocateUrb(VOID) {
  return AllocateZeroPool(sizeof(USB_REQUEST_BLOCK));
}

EFI_STATUS UsbGadgetInitUrb(
  USB_GADGET *Gadget,
  USB_REQUEST_BLOCK *Urb,
  VOID *Buffer,
  UINT32 Length,
  UINT32 Flags,
  USB_GADGET_REQUEST_COMPLETION_HANDLER Callback
) {
  if (!Gadget || !Urb)
    return EFI_INVALID_PARAMETER;
  
  if (Flags & ~(USB_FLAG_ZLP | USB_FLAG_TX))
    return EFI_INVALID_PARAMETER;

  Urb->Status = EFI_NOT_READY;
  Urb->Buffer = Buffer;
  Urb->Length = Length;
  Urb->Flags = Flags;
  Urb->Callback = Callback;

  return EFI_SUCCESS;
}

EFI_STATUS UsbGadgetQueueUrb(
  USB_GADGET *Gadget,
  UINT32 Endpoint,
  USB_REQUEST_BLOCK *Urb
) {
  if (!Gadget || !Urb)
    return EFI_INVALID_PARAMETER;

  return mUsb->Queue(mUsb, Endpoint, Urb);
}

EFI_STATUS UsbGadgetQueue(
  USB_GADGET *Gadget,
  UINT32 Endpoint,
  USB_REQUEST_BLOCK *Urb,
  VOID *Buffer,
  UINT32 Length,
  UINT32 Flags,
  USB_GADGET_REQUEST_COMPLETION_HANDLER Callback
) {
  EFI_STATUS Status;

  Status = UsbGadgetInitUrb(
    Gadget,
    Urb,
    Buffer,
    Length,
    Flags,
    Callback
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to initialize URB\n"));
    return Status;
  }

  return UsbGadgetQueueUrb(Gadget, Endpoint, Urb);
}

EFI_STATUS UsbGadgetEnableEndpoint(
  USB_GADGET *Gadget,
  USB_ENDPOINT_DESCRIPTOR *Endpoint
) {
  if (!Gadget || !Endpoint)
    return EFI_INVALID_PARAMETER;

  return mUsb->EnableEndpoint(mUsb, Endpoint);
}

EFI_STATUS UsbGadgetHaltEndpoint(USB_GADGET *Gadget, UINT32 Endpoint) {
  if (!Gadget)
    return EFI_INVALID_PARAMETER;

  return mUsb->Halt(mUsb, Endpoint, TRUE);
}

EFI_STATUS UsbGadgetUnHaltEndpoint(USB_GADGET *Gadget, UINT32 Endpoint) {
  if (!Gadget)
    return EFI_INVALID_PARAMETER;

  return mUsb->Halt(mUsb, Endpoint, FALSE);
}
