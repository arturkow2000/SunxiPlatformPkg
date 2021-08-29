#pragma once

// Types of functional descriptor
#define USB_CDC_FDT_HEADER 0x00
#define USB_CDC_FDT_CALL_MANAGEMENT 0x01
#define USB_CDC_FDT_ACM 0x02
#define USB_CDC_FDT_UNION 0x06

#define USB_CDC_SUBCLASS_ACM      0x02

#define USB_CDC_PROTO_NONE                0
#define USB_CDC_ACM_PROTO_AT_V25TER       1
#define USB_CDC_ACM_PROTO_VENDOR          0xff

#define USB_CDC_REQ_SET_LINE_CODING             0x20
#define USB_CDC_REQ_GET_LINE_CODING             0x21
#define USB_CDC_REQ_SET_CONTROL_LINE_STATE      0x22
#define USB_CDC_REQ_SEND_BREAK                  0x23

#pragma pack(1)

typedef struct {
  UINT8 Length;
  UINT8 DescriptorType;
  UINT8 DescriptorSubType;
  UINT16 BcdCDC;
} PACKED USB_CDC_FUNCTIONAL_DESCRIPTOR;

typedef struct {
  UINT8 Length;
  UINT8 DescriptorType;
  UINT8 DescriptorSubType;
  UINT8 Capabilities;
  UINT8 DataInterface;
} PACKED USB_CDC_CALL_MANAGEMENT_DESCRIPTOR;

typedef struct {
  UINT8 Length;
  UINT8 DescriptorType;
  UINT8 DescriptorSubType;
  UINT8 Capabilities;
} PACKED USB_CDC_ACM_DESCRIPTOR;

typedef struct {
  UINT8 Length;
  UINT8 DescriptorType;
  UINT8 DescriptorSubType;
  UINT8 MasterInterface;
  UINT8 SlaveInterface;
} PACKED USB_CDC_UNION_DESCRIPTOR;

#define USB_CDC_1_STOP_BITS 0
#define USB_CDC_1_5_STOP_BITS 1
#define USB_CDC_2_STOP_BITS 2
#define USB_CDC_NO_PARITY 0
#define USB_CDC_ODD_PARITY 1
#define USB_CDC_EVEN_PARITY 2
#define USB_CDC_MARK_PARITY 3
#define USB_CDC_SPACE_PARITY 4

typedef struct {
  UINT32 RteRate;
  UINT8 CharFormat;
  UINT8 ParityType;
  UINT8 DataBits;
} PACKED USB_CDC_LINE_CODING;


#pragma pack()
