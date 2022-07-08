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
    .__Length = sizeof(Text),                             \
    .__DescriptorType = USB_DESC_TYPE_STRING,             \
    .__String = Text,                                     \
  };

MAKE_STRING_DESCRIPTOR(mManufacturerString, L"Allwinner");
MAKE_STRING_DESCRIPTOR(mProductString, L"Sunxi device in UEFI mode");

#if 0
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
#endif

STATIC USB_DEVICE_DESCRIPTOR mDeviceDescriptor = {
  .Length = sizeof(USB_DEVICE_DESCRIPTOR),
  .DescriptorType = USB_DESC_TYPE_DEVICE,
#if FixedPcdGet32(Usb20Support)
  .BcdUSB = 0x200,
#else
  .BcdUSB = 0x110,
#endif
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

DEVICE_CONFIG gConfigDescriptorTemplate = {
  .Config = {
    .Length = sizeof(USB_CONFIG_DESCRIPTOR),
    .DescriptorType = USB_DESC_TYPE_CONFIG,
    .TotalLength = sizeof(DEVICE_CONFIG),
    .NumInterfaces = 3,
    .ConfigurationValue = 1,
    // Use same properties as FEL mode - bus powered + 300 mA
    .Attributes = (1 << 7),
    .MaxPower = (300 / 2)
  },
  .Custom = {
    .Length = sizeof(USB_INTERFACE_DESCRIPTOR),
    .DescriptorType = USB_DESC_TYPE_INTERFACE,
    .InterfaceNumber = CUSTOM_INTERFACE,
    .InterfaceClass = 255,
    .InterfaceSubClass = 255,
    .InterfaceProtocol = 255,
    .NumEndpoints = 2,
  },
  .CustomEpIn = {
    .Length = sizeof(USB_ENDPOINT_DESCRIPTOR),
    .DescriptorType = USB_DESC_TYPE_ENDPOINT,
    .Attributes = USB_ENDPOINT_BULK,
    .MaxPacketSize = CDC_DATA_MAX_PACKET,
  },
  .CustomEpOut = {
    .Length = sizeof(USB_ENDPOINT_DESCRIPTOR),
    .DescriptorType = USB_DESC_TYPE_ENDPOINT,
    .Attributes = USB_ENDPOINT_BULK,
    .MaxPacketSize = CDC_DATA_MAX_PACKET,
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
    .DescriptorType = USB_DESC_TYPE_INTERFACE,
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
    .DescriptorType = USB_DESC_TYPE_ENDPOINT,
    .Attributes = USB_ENDPOINT_INTERRUPT,
    .MaxPacketSize = CDC_DATA_MAX_PACKET,
    .Interval = 9,
  },
  .CdcData = {
    .Length = sizeof(USB_INTERFACE_DESCRIPTOR),
    .DescriptorType = USB_DESC_TYPE_INTERFACE,
    .InterfaceNumber = CDC_DATA_INTEFACE,
    .NumEndpoints = 2,
    .InterfaceClass = USB_CLASS_DATA,
    .InterfaceSubClass = 0,
    .InterfaceProtocol = 0,
  },
  .CdcDataEpIn = {
    .Length = sizeof(USB_ENDPOINT_DESCRIPTOR),
    .DescriptorType = USB_DESC_TYPE_ENDPOINT,
    .Attributes = USB_ENDPOINT_BULK,
    .MaxPacketSize = CDC_DATA_MAX_PACKET,
  },
  .CdcDataEpOut = {
    .Length = sizeof(USB_ENDPOINT_DESCRIPTOR),
    .DescriptorType = USB_DT_ENDPOINT,
    .Attributes = USB_ENDPOINT_BULK,
    .MaxPacketSize = CDC_DATA_MAX_PACKET,
  },
};

USB_DEVICE_DESCRIPTOR *UsbGadgetGetDeviceDescriptor(USB_GADGET *Gadget) {
  return &mDeviceDescriptor;
}

USB_CONFIG_DESCRIPTOR *UsbGadgetGetConfigDescriptor(USB_GADGET *Gadget) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(Gadget);
  return Driver->ConfigDescriptor;
}

STATIC UINT8 mLangListStringDescriptor[4] = {
  sizeof mLangListStringDescriptor,
  USB_DESC_TYPE_STRING,
  9,
  4
};

UINT8 *UsbGadgetGetStringDescriptor(USB_GADGET *Gadget, UINT8 Id) {
  switch (Id) {
  case 0:
    return mLangListStringDescriptor;
  case 0xEE:
    ASSERT(0);
    return NULL;
  case USB_STR_MANUFACTURER:
    return (UINT8*)&mManufacturerString;
  case USB_STR_PRODUCT:
    return (UINT8*)&mProductString;
  default:
    DEBUG((EFI_D_ERROR, "Unknown string descriptor #%d\n", Id));
    return NULL;
  }
}

EFI_STATUS UsbGadgetHandleClassRequest(USB_GADGET *This, USB_DEVICE_REQUEST *Request) {
  // TODO: check target endpoint/interface
  return CdcHandleRequest(This, Request);
}

EFI_STATUS UsbGadgetHandleVendorRequest(USB_GADGET *This, USB_DEVICE_REQUEST *Request) {
  DEBUG((EFI_D_ERROR, "UNHANDLED VENDOR REQUEST\n"));
  return EFI_UNSUPPORTED;
}

EFI_STATUS UsbGadgetHandleSetConfig(USB_GADGET *This, UINT8 Config) {
  EFI_STATUS Status;

  if (Config == 1) {
    Status = CdcEnable(This);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "CDC enable failed: %r\n", Status));
      return Status;
    }
    return EFI_SUCCESS;
  } else {
    // TODO: need to properly handle set config=0
    DEBUG((EFI_D_ERROR, "Unsupported config (%d) requested\n", Status));
    return EFI_UNSUPPORTED;
  }
}
