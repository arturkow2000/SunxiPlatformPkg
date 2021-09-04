#include "Driver.h"

#define USB_STR_MANUFACTURER 1
#define USB_STR_PRODUCT 2

#define MAKE_STRING_DESCRIPTOR(Name, Text)                \
  STATIC struct {                                         \
    UINT8 __Length;                                       \
    UINT8 __DescriptorType;                               \
    CHAR16 __String[sizeof(Text)];                        \
  } Name = {                                              \
    .__Length = 2 + sizeof(Text),                         \
    .__DescriptorType = USB_DESC_TYPE_STRING,             \
    .__String = Text,                                     \
  };

MAKE_STRING_DESCRIPTOR(mManufacturerString, L"Allwinner");
MAKE_STRING_DESCRIPTOR(mProductString, L"Sunxi device in PEI mode");

STATIC USB_DEVICE_DESCRIPTOR mDeviceDescriptor = {
  .Length = sizeof(USB_DEVICE_DESCRIPTOR),
  .DescriptorType = USB_DT_DEVICE,
  .BcdUSB = 0x200,
  .IdVendor = 0x1F3A,
  .IdProduct = 0x8E10,
  .NumConfigurations = 0,
  // Values required by USB IAD specification
  .DeviceClass = 0xEF,
  .DeviceSubClass = 0x02,
  .DeviceProtocol = 0x01,
  // From USB 2.0 spec:
  // If the device is operating at high-speed, the bMaxPacketSize0 field must be 64 indicating a 64 byte
  // maximum packet. High-speed operation does not allow other maximum packet sizes for the control
  // endpoint (endpoint 0).
  .MaxPacketSize0 = 64,
};

typedef struct _DEVICE_CONFIG {
  USB_CONFIG_DESCRIPTOR Config;
  USB_INTERFACE_ASSOCIATION_DESCRIPTOR Iad;
  USB_INTERFACE_DESCRIPTOR CdcAt;
  USB_CDC_FUNCTIONAL_DESCRIPTOR CdcFunctional;
  USB_CDC_CALL_MANAGEMENT_DESCRIPTOR CdcCallManagement;
  USB_CDC_ACM_DESCRIPTOR CdcAcm;
  USB_CDC_UNION_DESCRIPTOR CdcUnion;
  USB_ENDPOINT_DESCRIPTOR CdcControlEp;
  USB_INTERFACE_DESCRIPTOR CdcData;
  USB_ENDPOINT_DESCRIPTOR CdcDataEpIn;
  USB_ENDPOINT_DESCRIPTOR CdcDataEpOut;
} DEVICE_CONFIG;

STATIC DEVICE_CONFIG mConfigDescriptor = {
  .Config = {
    .Length = sizeof(USB_CONFIG_DESCRIPTOR),
    .DescriptorType = USB_DT_CONFIG,
    .TotalLength = sizeof(DEVICE_CONFIG),
    .NumInterfaces = 2,
    .ConfigurationValue = 1,
    // Use same properties as FEL mode - bus powered + 300 mA
    .Attributes = (1 << 7),
    .MaxPower = (300 / 2)
  },
  .Iad = {
    .Length = sizeof(USB_INTERFACE_ASSOCIATION_DESCRIPTOR),
    .DescriptorType = USB_DT_INTERFACE_ASSOCIATION,
    .FirstInterface = 0,
    .InterfaceCount = 2,
    .FunctionClass = USB_CLASS_COMM,
    .FunctionSubClass = USB_CDC_SUBCLASS_ACM,
    .FunctionProtocol = USB_CDC_ACM_PROTO_AT_V25TER
  },
  .CdcAt = {
    .Length = sizeof(USB_INTERFACE_DESCRIPTOR),
    .DescriptorType = USB_DT_INTERFACE,
    .InterfaceNumber = 0,
    .InterfaceClass = USB_CLASS_COMM,
    .InterfaceSubClass = USB_CDC_SUBCLASS_ACM,
    .InterfaceProtocol = USB_CDC_PROTO_NONE,
    .NumEndpoints = 1,
  },
  .CdcFunctional = {
    .Length = sizeof(USB_CDC_FUNCTIONAL_DESCRIPTOR),
    .DescriptorType = 0x24,
    .DescriptorSubType = USB_CDC_FDT_HEADER,
    .BcdCDC = 0x0110, // 1.10
  },
  .CdcCallManagement = {
    .Length = sizeof(USB_CDC_CALL_MANAGEMENT_DESCRIPTOR),
    .DescriptorType = 0x24,
    .DescriptorSubType = USB_CDC_FDT_CALL_MANAGEMENT,
    .Capabilities = 0,
    .DataInterface = 1,
  },
  .CdcAcm = {
    .Length = sizeof(USB_CDC_FDT_ACM),
    .DescriptorType = 0x24,
    .DescriptorSubType = USB_CDC_FDT_ACM,
    .Capabilities = 0x02
  },
  .CdcUnion = {
    .Length = sizeof(USB_CDC_UNION_DESCRIPTOR),
    .DescriptorType = 0x24,
    .DescriptorSubType = USB_CDC_FDT_UNION,
    .MasterInterface = 0,
    .SlaveInterface = 1,
  },
  .CdcControlEp = {
    .Length = sizeof(USB_ENDPOINT_DESCRIPTOR),
    .DescriptorType = 5,
    .EndpointAddress = 0x82,
    .Attributes = 3,
    .MaxPacketSize = 64,
    .Interval = 9,
  },
  .CdcData = {
    .Length = sizeof(USB_INTERFACE_DESCRIPTOR),
    .DescriptorType = USB_DT_INTERFACE,
    .InterfaceNumber = CDC_CONTROL_INTERFACE,
    .NumEndpoints = 2,
    .InterfaceClass = USB_CLASS_DATA,
    .InterfaceSubClass = 0,
    .InterfaceProtocol = 0,
  },
  .CdcDataEpIn = {
    .Length = sizeof(USB_ENDPOINT_DESCRIPTOR),
    .DescriptorType = USB_DT_ENDPOINT,
    .EndpointAddress = 0x81,
    .Attributes = 2,
    .MaxPacketSize = 512,
  },
  .CdcDataEpOut = {
    .Length = sizeof(USB_ENDPOINT_DESCRIPTOR),
    .DescriptorType = USB_DT_ENDPOINT,
    .EndpointAddress = 0x01,
    .Attributes = 2,
    .MaxPacketSize = 512,
  },
};

EFI_STATUS UsbGadgetSendStringDescriptor(
  GADGET_DRIVER_INTERNAL *Internal,
  UINT8 Id,
  UINT16 Language,
  UINT16 Length
) {
  STATIC UINT8 Buffer[4];
  VOID *String;
  UINT32 Size;
  UINT32 Flags = 0;

  if (Id == 0) {
    Buffer[0] = 4;
    Buffer[1] = USB_DT_STRING;
    // English
    Buffer[2] = 9;
    Buffer[3] = 4;

    return UsbGadgetEp0Queue(Internal, Buffer, Buffer[0], NULL, Length > 4 ? USB_PPI_FLAG_ZLP : 0);
  }

  switch (Id)
  {
  case USB_STR_MANUFACTURER:
    String = &mManufacturerString;
    Size = sizeof mManufacturerString;
    break;

  case USB_STR_PRODUCT:
    String = &mProductString;
    Size = sizeof mProductString;
    break;
  
  default:
    DEBUG((EFI_D_ERROR, "Unknown string descriptor #%d lang=0x%04x\n", Id, Language));

    // Stall
    return EFI_NOT_FOUND;
  }

  if (Size < Length)
    Flags |= USB_PPI_FLAG_ZLP;

  return UsbGadgetEp0Queue(Internal, String, Size, NULL, Flags);
}

EFI_STATUS UsbGadgetHandleStandardControlRequest(
  GADGET_DRIVER_INTERNAL *Internal,
  USB_DEVICE_REQUEST *Request
  )
{
  EFI_STATUS Status;
  UINT8 DescriptorIndex;

  if (Request->Request == USB_REQ_GET_DESCRIPTOR) {
    if (Request->RequestType != USB_ENDPOINT_DIR_IN)
      return EFI_DEVICE_ERROR;

    switch(Request->Value >> 8) {
    case USB_DT_DEVICE:
      DEBUG((EFI_D_ERROR, "----------------------------------------- DT_DEVICE len=%d zlp=%d\n", Request->Length, sizeof mDeviceDescriptor < Request->Length ? 1 : 0));
      return UsbGadgetEp0Queue(Internal, &mDeviceDescriptor, sizeof(mDeviceDescriptor), NULL, sizeof mDeviceDescriptor < Request->Length ? USB_PPI_FLAG_ZLP : 0);

    case USB_DT_CONFIG:
      DescriptorIndex = Request->Value;

      if (DescriptorIndex == 0)
        Status = UsbGadgetEp0Queue(Internal, &mConfigDescriptor, MIN(sizeof(mConfigDescriptor), Request->Length), NULL, 0);
      else {
        DEBUG((EFI_D_ERROR, "Unknown config descriptor index %d\n", DescriptorIndex));
        Status = EFI_DEVICE_ERROR;
      }

      return Status;

    case USB_DT_STRING:
      //return UsbGadgetSendStringDescriptor(Internal, Request->Value, 0x0000, Request->Length);
      return EFI_NOT_FOUND;

    default:
      DEBUG((EFI_D_ERROR, "Unknown descriptor type %d\n", Request->Value >> 8));
      break;
    }

    return EFI_DEVICE_ERROR;
  } else if (Request->Request == USB_REQ_SET_CONFIG) {
    if (Request->RequestType != 0)
      return EFI_DEVICE_ERROR;

    if (Request->Value != 1) {
      DEBUG((EFI_D_ERROR, "Invalid configuration requested (%d)\n", Request->Value));
      return EFI_DEVICE_ERROR;
    }

    // Signal configuration was set successfully
    return UsbGadgetEp0Queue(Internal, NULL, 0, NULL, 0);
  }

  return EFI_UNSUPPORTED;
}

EFI_STATUS UsbGadgetHandleControlRequest(
  USB_GADGET_DRIVER *This,
  USB_DRIVER *Driver,
  USB_DEVICE_REQUEST *Request
  )
{
  EFI_STATUS Status;
  GADGET_DRIVER_INTERNAL *Internal = GADGET_DRIVER_GET_INTERNAL(This);

  switch (Request->RequestType & USB_TYPE_MASK) {
  case USB_REQ_TYPE_STANDARD:
    Status = UsbGadgetHandleStandardControlRequest(Internal, Request);
    break;
  case USB_REQ_TYPE_CLASS:
    //Status = UsbGadgetHandleCdcRequest(Internal, Request);
    return EFI_DEVICE_ERROR;
    break;
  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}