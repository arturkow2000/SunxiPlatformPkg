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

#define USB_CLASS_PER_INTERFACE  0
#define USB_CLASS_AUDIO          1
#define USB_CLASS_COMM           2
#define USB_CLASS_HID            3
#define USB_CLASS_PRINTER        7
#define USB_CLASS_MASS_STORAGE   8
#define USB_CLASS_HUB            9
#define USB_CLASS_DATA           10
#define USB_CLASS_VENDOR_SPEC    0xff

#define USB_CDC_SUBCLASS_ACM      0x02
#define USB_CDC_SUBCLASS_ETHERNET 0x06
#define USB_CDC_SUBCLASS_WHCM     0x08
#define USB_CDC_SUBCLASS_DMM      0x09
#define USB_CDC_SUBCLASS_MDLM     0x0a
#define USB_CDC_SUBCLASS_OBEX     0x0b

#define USB_CDC_PROTO_NONE        0

#define USB_CDC_ACM_PROTO_AT_V25TER       1
#define USB_CDC_ACM_PROTO_AT_PCCA101      2
#define USB_CDC_ACM_PROTO_AT_PCCA101_WAKE 3
#define USB_CDC_ACM_PROTO_AT_GSM          4
#define USB_CDC_ACM_PROTO_AT_3G           5
#define USB_CDC_ACM_PROTO_AT_CDMA         6
#define USB_CDC_ACM_PROTO_VENDOR          0xff