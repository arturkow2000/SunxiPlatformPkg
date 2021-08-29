#pragma once

#include <IndustryStandard/Usb.h>

#define USB_TYPE_MASK         (3 << 5)

#define USB_RECIP_DEVICE      0x00
#define USB_RECIP_INTERFACE   0x01
#define USB_RECIP_ENDPOINT    0x02

#define USB_DT_DEVICE         0x01
#define USB_DT_CONFIG         0x02
#define USB_DT_STRING         0x03
#define USB_DT_INTERFACE      0x04
#define USB_DT_ENDPOINT       0x05
#define USB_DT_INTERFACE_ASSOCIATION    0x0b

#define USB_CLASS_PER_INTERFACE  0
#define USB_CLASS_AUDIO          1
#define USB_CLASS_COMM           2
#define USB_CLASS_HID            3
#define USB_CLASS_PRINTER        7
#define USB_CLASS_MASS_STORAGE   8
#define USB_CLASS_HUB            9
#define USB_CLASS_DATA           10
#define USB_CLASS_VENDOR_SPEC    0xff

typedef struct {
  UINT8 Length;
  UINT8 DescriptorType;
  UINT8 FirstInterface;
  UINT8 InterfaceCount;
  UINT8 FunctionClass;
  UINT8 FunctionSubClass;
  UINT8 FunctionProtocol;
  UINT8 Function;
} USB_INTERFACE_ASSOCIATION_DESCRIPTOR;
