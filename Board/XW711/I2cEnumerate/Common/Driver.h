#pragma once

#include <Pi/PiI2c.h>

#define EFI_I2C_DEVICE_TO_I2C_DEVICE(x) \
  CR(x, I2C_DEVICE, Device, I2C_DEVICE_SIGNATURE)
#define I2C_DEVICE_SIGNATURE SIGNATURE_32('i', '2', 'd', 'e')

typedef struct _I2C_DEVICE I2C_DEVICE;
struct _I2C_DEVICE {
  UINT32 Signature;
  EFI_I2C_DEVICE Device;
  I2C_DEVICE *Next;
};

extern CONST I2C_DEVICE *gI2cDeviceListHeadArray[];
extern CONST UINT32 gI2cDeviceListHeadArrayLength;