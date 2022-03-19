#include "../Driver.h"
#include <Library/SunxiGpioLib.h>

EFI_STATUS SunxiDisplayInitGpio(SUNXI_DISPLAY_DRIVER *Driver) {
  UINT32 i;

  if (FixedPcdGet32(LcdFlags) & LCD_PRESENT) {
    if (FixedPcdGet32(LcdInterface) == LCD_INTERFACE_PARALLEL) {
      for (i = 0; i < 27; i++)
        SunxiGpioSetFunction(SUNXI_GPIO_PIN_PD(i), L"lcd0");
    } else {
      ASSERT(0);
      return EFI_UNSUPPORTED;
    }
  }

  return EFI_SUCCESS;
}
