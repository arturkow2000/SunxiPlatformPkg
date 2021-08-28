#include "Driver.h"

#define USB_STR_MANUFACTURER 1
#define USB_STR_PRODUCT 1
#define USB_STR_ACM_INTERFACE_DESC 2
#define USB_STR_DATA_INTERFACE_DESC 3

#define MAKE_STRING_DESCRIPTOR(InternalName, Name, Text)  \
  struct InternalName {                                   \
    UINT8 __Length;                                       \
    UINT8 __DescriptorType;                               \
    CHAR16 __String[sizeof(Text)];                        \
  };                                                      \
  STATIC struct InternalName Name = {                     \
    .__Length = 2 + sizeof(Text),                         \
    .__DescriptorType = USB_DT_STRING,                    \
    .__String = Text,                                     \
  };

MAKE_STRING_DESCRIPTOR(__STRING_DESC_MANUFACTURER, mManufacturerString, L"Hehe");

STATIC USB_DEVICE_DESCRIPTOR mDeviceDescriptor = {
  .Length = sizeof(USB_DEVICE_DESCRIPTOR),
  .DescriptorType = USB_DT_DEVICE,
  .BcdUSB = 0x200,
  .IdVendor = 0x4444,
  .IdProduct = 0x5555,
  .StrManufacturer = USB_STR_MANUFACTURER,
  .StrProduct = USB_STR_PRODUCT,
  .NumConfigurations = 1,
  // FIXME: should fill this basing on data provided by USB controller driver
  .MaxPacketSize0 = 64
};

typedef struct _DEVICE_CONFIG {
  USB_CONFIG_DESCRIPTOR Config;
  USB_INTERFACE_DESCRIPTOR AcmControl;
  //USB_INTERFACE_DESCRIPTOR AcmData;
} DEVICE_CONFIG;

STATIC DEVICE_CONFIG mConfigDescriptor = {
  .Config = {
    .Length = sizeof(USB_CONFIG_DESCRIPTOR),
    .DescriptorType = USB_DT_CONFIG,
    .TotalLength = sizeof(DEVICE_CONFIG),
    .NumInterfaces = 1,
    .ConfigurationValue = 1,
    .Configuration = 2,
    .Attributes = (1 << 7) | (1 << 6),
    .MaxPower = 1
  },
  .AcmControl = {
    .Length = sizeof(USB_INTERFACE_DESCRIPTOR),
    .DescriptorType = USB_DT_INTERFACE,
    .InterfaceNumber = 0,
    .InterfaceClass = USB_CLASS_COMM,
    .InterfaceSubClass = USB_CDC_SUBCLASS_ACM,
    .InterfaceProtocol = USB_CDC_PROTO_NONE,
    .Interface = USB_STR_ACM_INTERFACE_DESC
  },
  /*.AcmData = {
    .Length = sizeof(USB_INTERFACE_DESCRIPTOR),
    .DescriptorType = USB_DT_INTERFACE,
    .InterfaceNumber = 1,
    .InterfaceClass = USB_CLASS_DATA,
    .InterfaceSubClass = 0,
    .InterfaceProtocol = 0,
    .Interface = USB_STR_DATA_INTERFACE_DESC
  },*/
};

/*STATIC USB_CONFIG_DESCRIPTOR mConfigDescriptor = {
  .Length = sizeof(USB_CONFIG_DESCRIPTOR),
  .DescriptorType = USB_DT_CONFIG,
  .TotalLength = sizeof(USB_CONFIG_DESCRIPTOR),
  .NumInterfaces = 0,
  .ConfigurationValue = 1,
  .Configuration = 2,
  .Attributes = (1 << 7) | (1 << 6),
  .MaxPower = 1
};*/

EFI_STATUS UsbGadgetEp0Write(GADGET_DRIVER_INTERNAL *Internal, VOID *Buffer, UINT32 Length) {
  EFI_STATUS Status;

  Status = Internal->Usb->InitRequest(
    Internal->Usb,
    Internal->ControlRequest,
    Buffer,
    Length,
    0,
    &Internal->ControlRequestStatus
  );

  if (EFI_ERROR(Status))
    return Status;
    
  return Internal->Usb->QueueEp0Packet(
    Internal->Usb,
    Internal->ControlRequest
  );
}

EFI_STATUS UsbGadgetSendStringDescriptor(
  GADGET_DRIVER_INTERNAL *Internal,
  UINT8 Id,
  UINT16 Language
) {
  STATIC UINT8 Buffer[4];

  if (Id == 0) {
    Buffer[0] = 4;
    Buffer[1] = USB_DT_STRING;
    // English
    Buffer[2] = 9;
    Buffer[3] = 4;

    return UsbGadgetEp0Write(Internal, Buffer, Buffer[0]);
  }

  switch (Id)
  {
  case 1:
    return UsbGadgetEp0Write(Internal, &mManufacturerString, sizeof(mManufacturerString));
  
  default:
    DEBUG((EFI_D_ERROR, "Unknown string descriptor #%d lang=0x%04x\n", Id, Language));

    // Stall
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS UsbGadgetHandleControlRequest(
  USB_GADGET_DRIVER *This,
  USB_DRIVER *Driver,
  USB_DEVICE_REQUEST *Request
  )
{
  GADGET_DRIVER_INTERNAL *Internal = GADGET_DRIVER_GET_INTERNAL(This);
  EFI_STATUS Status;

  if (Request->Request == USB_REQ_GET_DESCRIPTOR) {
    switch(Request->Value >> 8) {
    case USB_DT_DEVICE:
      DEBUG((EFI_D_INFO, "Sending device descriptor\n"));
      ASSERT(Internal->ControlRequestStatus = 1);
      Status = Internal->Usb->InitRequest(
        Internal->Usb,
        Internal->ControlRequest,
        (VOID*)&mDeviceDescriptor,
        sizeof(USB_DEVICE_DESCRIPTOR),
        0,
        &Internal->ControlRequestStatus
      );
      ASSERT_EFI_ERROR(Status);

      Status = Internal->Usb->QueueEp0Packet(
        Internal->Usb,
        Internal->ControlRequest
      );
      ASSERT_EFI_ERROR(Status);
      return Status;
    
    case USB_DT_CONFIG:
      DEBUG((EFI_D_INFO, "Sending config descriptor\n"));
      ASSERT(Internal->ControlRequestStatus == 1);
      Status = Internal->Usb->InitRequest(
        Internal->Usb,
        Internal->ControlRequest,
        (VOID*)&mConfigDescriptor,
        sizeof(mConfigDescriptor),
        0,
        &Internal->ControlRequestStatus
      );
      ASSERT_EFI_ERROR(Status);

      Status = Internal->Usb->QueueEp0Packet(
        Internal->Usb,
        Internal->ControlRequest
      );
      ASSERT_EFI_ERROR(Status);
      return Status;

    case USB_DT_STRING:
      return UsbGadgetSendStringDescriptor(Internal, Request->Value, 0x0000);
    default:
      DEBUG((EFI_D_ERROR, "Unknown descriptor type %d\n", Request->Value >> 8));
      break;
    }

    return EFI_DEVICE_ERROR;
  }

  return EFI_UNSUPPORTED;
}