#pragma once

#define I2C_ENUMERATE_PPI_GUID                                                \
  {                                                                                 \
    0x48ef0da1, 0x13d7, 0x48df, { 0xbe, 0x79, 0x09, 0x81, 0x8b, 0x40, 0x5e, 0x18 }  \
  }

typedef struct _I2C_ENUMERATE_PPI I2C_ENUMERATE_PPI;

typedef
EFI_STATUS
(EFIAPI *I2C_ENUMERATE_PPI_ENUMERATE)(
  IN CONST I2C_ENUMERATE_PPI          *This,
  IN UINT32                           Controller,
  IN OUT CONST EFI_I2C_DEVICE         **Device
);

struct _I2C_ENUMERATE_PPI {
  I2C_ENUMERATE_PPI_ENUMERATE Enumerate;
};

extern EFI_GUID gI2cEnumeratePpiGuid;
