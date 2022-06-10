#include "Gadget.h"

STATIC EFI_STATUS HandleStandardRequest(
  USB_GADGET *Gadget,
  USB_DEVICE_REQUEST *Request
) {
  EFI_STATUS Status;

  if (Request->Request == USB_REQ_GET_DESCRIPTOR) {
    if (Request->RequestType != USB_ENDPOINT_DIR_IN)
      return EFI_DEVICE_ERROR;

    switch(Request->Value >> 8) {
    case USB_DESC_TYPE_DEVICE:
      ASSERT(Gadget->GetDeviceDescriptor);
      USB_DEVICE_DESCRIPTOR *DeviceDescriptor = Gadget->GetDeviceDescriptor(Gadget);
      if (!DeviceDescriptor)
        return EFI_DEVICE_ERROR;

      ASSERT(DeviceDescriptor->DescriptorType == USB_DESC_TYPE_DEVICE);

      return UsbGadgetInitUrb(
        Gadget,
        Gadget->ControlUrb,
        DeviceDescriptor,
        DeviceDescriptor->Length,
        DeviceDescriptor->Length % 64 == 0 ? USB_FLAG_ZLP : 0 | USB_FLAG_TX,
        NULL
      );
    case USB_DESC_TYPE_CONFIG:
      ASSERT(Gadget->GetConfigDescriptor);
      UINT32 TotalLength;
      USB_CONFIG_DESCRIPTOR *ConfigDescriptor = Gadget->GetConfigDescriptor(Gadget);
      if (!ConfigDescriptor)
        return EFI_DEVICE_ERROR;

      ASSERT(ConfigDescriptor->DescriptorType == USB_DESC_TYPE_CONFIG);

      TotalLength = ConfigDescriptor->TotalLength;
      TotalLength = MIN(TotalLength, Request->Length);
      return UsbGadgetInitUrb(
        Gadget,
        Gadget->ControlUrb,
        ConfigDescriptor,
        TotalLength,
        TotalLength % 64 == 0 ? USB_FLAG_ZLP : 0 | USB_FLAG_TX,
        NULL
      );
    case USB_DESC_TYPE_STRING:
      ASSERT(Gadget->GetStringDescriptor);
      UINT8 *StringDescriptor = Gadget->GetStringDescriptor(Gadget, Request->Value);
      if (!StringDescriptor)
        return EFI_DEVICE_ERROR;

      ASSERT(StringDescriptor[1] == USB_DESC_TYPE_STRING);

      return UsbGadgetInitUrb(
        Gadget,
        Gadget->ControlUrb,
        StringDescriptor,
        StringDescriptor[0],
        StringDescriptor[0] % 64 == 0 ? USB_FLAG_ZLP : 0 | USB_FLAG_TX,
        NULL
      );
    default:
      DEBUG((EFI_D_ERROR, "Unknown descriptor type requested (%d)\n", Request->Value >> 8));
      return EFI_DEVICE_ERROR;
    }
  } else if (Request->Request == USB_REQ_SET_CONFIG) {
    if (Request->RequestType != 0)
      return EFI_DEVICE_ERROR;

    Status = Gadget->HandleSetConfig(Gadget, Request->Value & 0xff);
    if (EFI_ERROR(Status))
      return Status;

    return UsbGadgetInitUrb(Gadget, Gadget->ControlUrb, NULL, 0, 0, NULL);
  } else {
    DEBUG((EFI_D_ERROR, "Unknown request %d\n", Request->Request));
    ASSERT(0);
  }

  return EFI_DEVICE_ERROR;
}

EFI_STATUS UsbGadgetHandleControlRequest(
  USB_GADGET_INTERFACE *This,
  USB_DEVICE_REQUEST *Request
) {
  USB_GADGET* Gadget = INTERFACE_TO_GADGET(This);
  EFI_STATUS Status = EFI_DEVICE_ERROR;
  ASSERT(This);
  ASSERT(Request);

  switch (Request->RequestType & USB_TYPE_MASK) {
  case USB_REQ_TYPE_STANDARD:
    Status = HandleStandardRequest(Gadget, Request);
    break;
  case USB_REQ_TYPE_CLASS:
    Status = Gadget->HandleClassRequest(Gadget, Request);
    break;
  case USB_REQ_TYPE_VENDOR:
    Status = Gadget->HandleVendorRequest(Gadget, Request);
    break;
  }

  if (Status == EFI_SUCCESS) {
    DEBUG((
      EFI_D_INFO,
      "Queue URB (ep0) %p len %d\n",
      Gadget->ControlUrb->Buffer,
      Gadget->ControlUrb->Length
    ));
    Status = UsbGadgetQueueUrb(Gadget, 0, Gadget->ControlUrb);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status))
      DEBUG((EFI_D_ERROR, "Failed to queue URB: %r\n", Status));
  }

  return Status;
}
