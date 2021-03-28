/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on code Linux code
 Copyright (C) 2010-2015 Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 Pan Nan <pannan@allwinnertech.com>
 Tom Cubie <tanglaing@allwinnertech.com>
 Victor Wei <weiziheng@allwinnertech.com>

**/

#include "Driver.h"

// FIXME: move board specific stuff somewhere else

#if FixedPcdGet32(I2c0Base) != 0
STATIC CONST GPIO_CONFIG mI2c0GpioConfig[] = {
  { L"PB00", L"i2c0", 1, 20 },
  { L"PB01", L"i2c0", 1, 20 },
};
#endif

#if FixedPcdGet32(I2c1Base) != 0
STATIC CONST GPIO_CONFIG mI2c1GpioConfig[] = {
  { L"PB15", L"i2c1", 1, 20 },
  { L"PB16", L"i2c1", 1, 20 },
};
#endif

#if FixedPcdGet32(I2c2Base) != 0
STATIC CONST GPIO_CONFIG mI2c2GpioConfig[] = {
  { L"PB17", L"i2c2", 1, 20 },
  { L"PB18", L"i2c2", 1, 20 },
};
#endif

#if FixedPcdGet32(I2c3Base) != 0
#error not supported
#endif

#if FixedPcdGet32(I2c4Base) != 0
#error not supported
#endif

SUNXI_I2C_CONFIG gSunxiI2cConfig[] = {
#if FixedPcdGet32(I2c0Base) != 0
  {
    mI2c0GpioConfig,
    ARRAY_SIZE(mI2c0GpioConfig),
    FixedPcdGet32(I2c0Base),
    0
  },
#endif
#if FixedPcdGet32(I2c1Base) != 0
  {
    mI2c1GpioConfig,
    ARRAY_SIZE(mI2c1GpioConfig),
    FixedPcdGet32(I2c1Base),
    1
  },
#endif
#if FixedPcdGet32(I2c2Base) != 0
  {
    mI2c2GpioConfig,
    ARRAY_SIZE(mI2c2GpioConfig),
    FixedPcdGet32(I2c2Base),
    2
  },
#endif
#if FixedPcdGet32(I2c3Base) != 0
  {
    mI2c3GpioConfig,
    ARRAY_SIZE(mI2c3GpioConfig),
    FixedPcdGet32(I2c3Base),
    3
  },
#endif
#if FixedPcdGet32(I2c4Base) != 0
  {
    mI2c4GpioConfig,
    ARRAY_SIZE(mI2c4GpioConfig),
    FixedPcdGet32(I2c4Base),
    4
  }
#endif
};

UINT32 gSunxiI2cNumControllers = ARRAY_SIZE(gSunxiI2cConfig);
