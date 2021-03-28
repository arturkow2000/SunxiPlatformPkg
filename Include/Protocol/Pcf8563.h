#pragma once

#define PCF8563_PROTOCOL_GUID                                                       \
  {                                                                                 \
    0x859dd7ab, 0xe8d0, 0x4312, { 0x83, 0xb4, 0x5b, 0x95, 0x38, 0x04, 0x0a, 0xca }  \
  }

typedef struct _PCF8563_PROTOCOL PCF8563_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *PCF8563_GET_TIME)(
  IN  PCF8563_PROTOCOL       *This,
  OUT EFI_TIME               *Time,
  OUT EFI_TIME_CAPABILITIES  *Capabilities
  );

typedef
EFI_STATUS
(EFIAPI *PCF8563_SET_TIME)(
  IN  PCF8563_PROTOCOL       *This,
  IN  EFI_TIME               *Time
  );
  
struct _PCF8563_PROTOCOL {
  PCF8563_GET_TIME GetTime;
  PCF8563_SET_TIME SetTime;
};

extern EFI_GUID gPcf8563ProtocolGuid;
