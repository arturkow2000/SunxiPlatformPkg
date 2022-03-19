#include "Pwm.h"

UINT32 SunxiPwmGetPrescaler(UINT32 Frequency) {
  switch (Frequency) {
  case SUNXI_PWM_FREQ_24MHZ: return 0xf;
  default:
    // Currently we don't use any other frequencies.
    return 0xffffffff;
  }
}