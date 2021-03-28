#include "Driver.h"

extern EFI_GUID gI2cAxp209Guid;
extern EFI_GUID gI2cPcf8563Guid;

STATIC CONST UINT32 mAxp209SlaveAddressList[] = { 0x34 };
STATIC CONST I2C_DEVICE mAxp209 = {
  I2C_DEVICE_SIGNATURE,
  {
    &gI2cAxp209Guid,
    0,
    ///
    /// Hardware revision - ACPI _HRV value.  See the Advanced
    /// Configuration and Power Interface Specification, Revision 5.0
    /// for the field format and the Plug and play support for I2C
    /// web-page for restriction on values.
    ///
    /// http://www.acpi.info/spec.htm
    /// http://msdn.microsoft.com/en-us/library/windows/hardware/jj131711(v=vs.85).aspx
    ///
    // TODO: verify hardware revision
    0,
    0,
    1,
    mAxp209SlaveAddressList
  },
  NULL
};

STATIC CONST UINT32 mPcf8563SlaveAddressList[] = { 0x51 };
STATIC CONST I2C_DEVICE mPcf8563 = {
  I2C_DEVICE_SIGNATURE,
  {
    &gI2cPcf8563Guid,
    0,
    0,
    0,
    1,
    mPcf8563SlaveAddressList,
  },
  NULL
};

CONST I2C_DEVICE *gI2cDeviceListHeadArray[] = {
  &mAxp209,
  &mPcf8563,
};

CONST UINT32 gI2cDeviceListHeadArrayLength = ARRAY_SIZE(gI2cDeviceListHeadArray);
