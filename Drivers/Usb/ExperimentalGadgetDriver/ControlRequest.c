#include "Driver.h"

#define USB_STR_MANUFACTURER 1
#define USB_STR_PRODUCT 2

#define MOS_VENDOR_CODE 0xac

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

STATIC struct {
  UINT8 __Length;
  UINT8 __DescriptorType;
  CHAR8 __String[16];
} mMsftString = {
  18, 3,
  { 'M', 0, 'S', 0, 'F', 0, 'T', 0, '1', 0, '0', 0, '0', 0, MOS_VENDOR_CODE, 0x00 }
};

STATIC MICROSOFT_FEATURE_DESCRIPTOR mMicrosoftFeatureDescriptor = {
  .Length = sizeof(MICROSOFT_FEATURE_DESCRIPTOR),
  .Version = MICROSOFT_FEATURE_DESCRIPTOR_VERSION,
  .CompatibilityId = 4,
  .NumberOfSections = 1,
  .Reserved0 = { 0, 0, 0, 0, 0, 0, 0 },
  .InterfaceNumber = CUSTOM_INTERFACE,
  .One = 1,
  .CompatibleId = { 'W', 'I', 'N', 'U', 'S', 'B', 0, 0 },
  .SubCompatibleId = { 0, 0, 0, 0, 0, 0, 0, 0 },
  .Reserved1 = { 0, 0, 0, 0, 0, 0 },
};

STATIC USB_DEVICE_DESCRIPTOR mDeviceDescriptor = {
  .Length = sizeof(USB_DEVICE_DESCRIPTOR),
  .DescriptorType = USB_DT_DEVICE,
  .BcdUSB = 0x200,
  .IdVendor = 0x1F3A,
  .IdProduct = 0x8E10,
  .NumConfigurations = 1,
  // These must be zero for composite device
  .DeviceClass = 0x00,
  .DeviceSubClass = 0x00,
  .DeviceProtocol = 0x00,
  // From USB 2.0 spec:
  // If the device is operating at high-speed, the bMaxPacketSize0 field must be 64 indicating a 64 byte
  // maximum packet. High-speed operation does not allow other maximum packet sizes for the control
  // endpoint (endpoint 0).
  .MaxPacketSize0 = 64,
  .BcdDevice = 0x0001,
  .StrManufacturer = USB_STR_MANUFACTURER,
  .StrProduct = USB_STR_PRODUCT
};

typedef struct _DEVICE_CONFIG {
  USB_CONFIG_DESCRIPTOR Config;
  // Interface with custom protocol used for controlling device
  // currently dummy,
  // it will be used to transfer EFI image when booting from USB
  // and possibly for debugging purposes
  //
  // CDC will be used to emulate serial port
  USB_INTERFACE_DESCRIPTOR Custom;
  USB_INTERFACE_ASSOCIATION_DESCRIPTOR Iad0;
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
    .NumInterfaces = 3,
    .ConfigurationValue = 1,
    // Use same properties as FEL mode - bus powered + 300 mA
    .Attributes = (1 << 7),
    .MaxPower = (300 / 2)
  },
  .Custom = {
    .Length = sizeof(USB_INTERFACE_DESCRIPTOR),
    .DescriptorType = USB_DT_INTERFACE,
    .InterfaceNumber = CUSTOM_INTERFACE,
    .InterfaceClass = 255,
    .InterfaceSubClass = 255,
    .InterfaceProtocol = 255,
  },
  .Iad0 = {
    .Length = sizeof(USB_INTERFACE_ASSOCIATION_DESCRIPTOR),
    .DescriptorType = USB_DT_INTERFACE_ASSOCIATION,
    .FirstInterface = CDC_CONTROL_INTERFACE,
    .InterfaceCount = 2,
    .FunctionClass = USB_CLASS_COMM,
    .FunctionSubClass = USB_CDC_SUBCLASS_ACM,
    .FunctionProtocol = USB_CDC_ACM_PROTO_AT_V25TER
  },
  .CdcAt = {
    .Length = sizeof(USB_INTERFACE_DESCRIPTOR),
    .DescriptorType = USB_DT_INTERFACE,
    .InterfaceNumber = CDC_CONTROL_INTERFACE,
    .InterfaceClass = USB_CLASS_COMM,
    .InterfaceSubClass = USB_CDC_SUBCLASS_ACM,
    .InterfaceProtocol = USB_CDC_ACM_PROTO_AT_V25TER,
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
    .DataInterface = CDC_DATA_INTEFACE,
  },
  .CdcAcm = {
    .Length = sizeof(USB_CDC_ACM_DESCRIPTOR),
    .DescriptorType = 0x24,
    .DescriptorSubType = USB_CDC_FDT_ACM,
    .Capabilities = 0x02
  },
  .CdcUnion = {
    .Length = sizeof(USB_CDC_UNION_DESCRIPTOR),
    .DescriptorType = 0x24,
    .DescriptorSubType = USB_CDC_FDT_UNION,
    .MasterInterface = CDC_CONTROL_INTERFACE,
    .SlaveInterface = CDC_DATA_INTEFACE,
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
    .InterfaceNumber = CDC_DATA_INTEFACE,
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
  USB_DEVICE_REQUEST *Request,
  UINT8 Id,
  UINT16 Language
) {
  STATIC UINT8 Buffer[4];

  if (Id == 0) {
    Buffer[0] = sizeof Buffer;
    Buffer[1] = USB_DT_STRING;
    // English
    Buffer[4] = 9;
    Buffer[5] = 4;

    return UsbGadgetEp0Respond(Internal, Request, Buffer, sizeof Buffer);
  }

  switch (Id)
  {
  case 0xEE:
    return UsbGadgetEp0Respond(Internal, Request, &mMsftString, sizeof mMsftString);

  case USB_STR_MANUFACTURER:
    return UsbGadgetEp0Respond(Internal, Request, &mManufacturerString, sizeof mManufacturerString);

  case USB_STR_PRODUCT:
    return UsbGadgetEp0Respond(Internal, Request, &mProductString, sizeof mProductString);
  
  default:
    DEBUG((EFI_D_ERROR, "Unknown string descriptor #%d lang=0x%04x\n", Id, Language));

    // Stall
    return EFI_NOT_FOUND;
  }
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
      return UsbGadgetEp0Respond(Internal, Request, &mDeviceDescriptor, sizeof mDeviceDescriptor);

    case USB_DT_CONFIG:
      DescriptorIndex = Request->Value;

      if (DescriptorIndex == 0)
        Status = UsbGadgetEp0Respond(Internal, Request, &mConfigDescriptor, sizeof mConfigDescriptor);
      else {
        DEBUG((EFI_D_ERROR, "Unknown config descriptor index %d\n", DescriptorIndex));
        Status = EFI_DEVICE_ERROR;
      }

      return Status;

    case USB_DT_STRING:
      return UsbGadgetSendStringDescriptor(Internal, Request, Request->Value, 0x0000);

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

STATIC EFI_STATUS HandleMosRequest(
  GADGET_DRIVER_INTERNAL *Internal,
  USB_DEVICE_REQUEST *Request
  )
{
  UINT8 Target = Request->RequestType & 0x1f;

  if (Target != USB_TARGET_DEVICE || !(Request->RequestType & USB_ENDPOINT_DIR_IN))
    return EFI_UNSUPPORTED;

  return UsbGadgetEp0Respond(Internal, Request, &mMicrosoftFeatureDescriptor, sizeof mMicrosoftFeatureDescriptor);
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
    Status = UsbGadgetHandleCdcRequest(Internal, Request);
    break;
  case USB_REQ_TYPE_VENDOR:
    if (Request->Request == MOS_VENDOR_CODE && Request->Index == 0x04)
      return HandleMosRequest(Internal, Request);
    /* fallthrough */
  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}