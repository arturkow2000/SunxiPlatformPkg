#pragma once

#define AXP_POWER_PROTOCOL_GUID \
  { \
    0x6834fe45, 0x682c, 0xc64f, { 0xa6, 0x8f, 0x55, 0x2d, 0xe8, 0x4f, 0x43, 0x7e } \
  }

typedef struct _AXP_POWER_PROTOCOL AXP_POWER_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *AXP_POWER_GPIO_DIRECTION_OUTPUT)(
  IN AXP_POWER_PROTOCOL      *This,
  IN UINTN                   Pin,
  IN UINTN                   Value
  );

typedef
EFI_STATUS
(EFIAPI *AXP_POWER_GPIO_DIRECTION_INPUT)(
  IN AXP_POWER_PROTOCOL      *This,
  IN UINTN                   Pin
  );

typedef
EFI_STATUS
(EFIAPI *AXP_POWER_GPIO_GET_VALUE)(
  IN AXP_POWER_PROTOCOL      *This,
  IN UINTN                   Pin,
  OUT UINTN                  *Value
  );

typedef
EFI_STATUS
(EFIAPI *AXP_POWER_GPIO_SET_VALUE)(
  IN AXP_POWER_PROTOCOL      *This,
  IN UINTN                   Pin,
  IN UINTN                   Value
  );

typedef
EFI_STATUS
(EFIAPI *AXP_POWER_SHUTDOWN)(
  IN AXP_POWER_PROTOCOL      *This
  );

struct _AXP_POWER_PROTOCOL {
  AXP_POWER_GPIO_DIRECTION_OUTPUT GpioDirectionOutput;
  AXP_POWER_GPIO_DIRECTION_INPUT GpioDirectionInput;
  AXP_POWER_GPIO_GET_VALUE GpioGetValue;
  AXP_POWER_GPIO_SET_VALUE GpioSetValue;
  AXP_POWER_SHUTDOWN Shutdown;

  EFI_DEVICE_PATH_PROTOCOL *DevicePath;
  EFI_I2C_IO_PROTOCOL *I2cIo;
};

extern EFI_GUID gAxpPowerProtocolGuid;