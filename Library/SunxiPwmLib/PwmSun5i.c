#include "Pwm.h"
#include <Library/SunxiGpioLib.h>

#define ALL_VALID_FLAGS (SUNXI_PWM_ACTIVE_LOW)

EFI_STATUS SunxiPwmGetChannelCount(
  UINT32 *OutChannelCount
  )
{
  if (!OutChannelCount)
    return EFI_INVALID_PARAMETER;

  // We don't need more channels. Currently only CH0 is used for controlling
  // display backlight.
  *OutChannelCount = 1;
  return EFI_SUCCESS;
}

EFI_STATUS SunxiPwmPrepareChannel(UINT32 Channel) {
  EFI_STATUS Status;
  UINT32 Pin;

  switch (Channel)
  {
  case 0:
    Pin = SUNXI_GPIO_PIN_PB(2);
    break;
  
  default:
    return EFI_NOT_FOUND;
  }

  Status = SunxiGpioSetFunction(Pin, L"pwm");
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return EFI_DEVICE_ERROR;

  return EFI_SUCCESS;
}

EFI_STATUS SunxiPwmConfigureChannel(
  UINT32 Channel,
  UINT32 Frequency,
  UINT32 EntirePeriod,
  UINT32 ActivePeriod,
  UINT32 Flags
) {
  UINT32 Prescaler;
  UINT32 Reg;
  UINT32 Control;
  UINT32 OldPrescaler;

  switch (Channel) {
  case 0:
    Reg = PWM_CH0;
    break;
  default:
    return EFI_NOT_FOUND;
  }

  if (Frequency == 0) {
    // Disable PWM
    Control = MmioRead32(PWM_CTRL);
    Control &= ~PWM_CTRL_CH_ENABLE(Channel);
    MmioWrite32(PWM_CTRL, Control);
    return EFI_SUCCESS;
  }

  Prescaler = SunxiPwmGetPrescaler(Frequency);
  if (Prescaler == 0xffffffff)
    return EFI_INVALID_PARAMETER;

  if (EntirePeriod == 0)
    return EFI_INVALID_PARAMETER;
  EntirePeriod -= 1;

  if ((EntirePeriod & ~0xFFFF) != 0 || (ActivePeriod & ~0xFFFF) != 0)
    return EFI_INVALID_PARAMETER;

  if (ActivePeriod > EntirePeriod)
    return EFI_INVALID_PARAMETER;

  if (Flags & ~ALL_VALID_FLAGS)
    return EFI_INVALID_PARAMETER;

  Control = MmioRead32(PWM_CTRL);

  OldPrescaler = (Control & PWM_CTRL_CH_PRESCALER_MASK(Channel)) >> PWM_CTRL_CH_PRESCALER_SHIFT(Channel);
  if (OldPrescaler != Prescaler) {
    if (Control & PWM_CTRL_CH_GATING(Channel)) {
      // Clock must be gated while changing prescaler
      Control &= ~PWM_CTRL_CH_GATING(Channel);
      MmioWrite32(PWM_CTRL, Control);
    }

    Control &= ~PWM_CTRL_CH_PRESCALER_MASK(Channel);
    Control |= Prescaler << PWM_CTRL_CH_PRESCALER_SHIFT(Channel);
  }

  MmioWrite32(Reg, (EntirePeriod << 16) | ActivePeriod);
  if (Flags & SUNXI_PWM_ACTIVE_LOW)
    Control &= ~PWM_CTRL_CH_POLARITY(Channel);
  else
    Control |= PWM_CTRL_CH_POLARITY(Channel);

  Control |= PWM_CTRL_CH_GATING(Channel) | PWM_CTRL_CH_ENABLE(Channel);
  MmioWrite32(PWM_CTRL, Control);

  return EFI_SUCCESS;
}
