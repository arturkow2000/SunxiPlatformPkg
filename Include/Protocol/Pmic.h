#pragma once

#define PMIC_PROTOCOL_GUID                                                          \
  {                                                                                 \
    0x9e61b85f, 0xdd43, 0x4d6b, { 0xaf, 0xc8, 0x70, 0xce, 0x41, 0x07, 0x4b, 0xf1 }  \
  }

typedef struct _PMIC_PROTOCOL PMIC_PROTOCOL;

// Poweroff device
typedef
EFI_STATUS
(EFIAPI *PMIC_POWEROFF)(
  IN  PMIC_PROTOCOL     *This
  );

struct _PMIC_PROTOCOL {
  PMIC_POWEROFF Poweroff;
};

extern EFI_GUID gPmicProtocolGuid;
