#pragma once

#include <Uefi.h>

#define SUNXI_PWM_FREQ_24MHZ 24000000
#define SUNXI_PWM_FREQ_200KHZ 200000
#define SUNXI_PWM_FREQ_180KHZ 180000
#define SUNXI_PWM_FREQ_100KHZ 100000
#define SUNXI_PWM_FREQ_67KHZ 66666
#define SUNXI_PWM_FREQ_50KHZ 50000
#define SUNXI_PWM_FREQ_2KHZ 2000
#define SUNXI_PWM_FREQ_1KHZ 1000
#define SUNXI_PWM_FREQ_667HZ 667
#define SUNXI_PWM_FREQ_500HZ 500
#define SUNXI_PWM_FREQ_333HZ 333

#define SUNXI_PWM_ACTIVE_LOW (1 << 0)

/**
 Returns number of supported channels.

 @retval EFI_SUCCESS              Channel count returned.
 @retval EFI_INVALID_PARAMETER    OutChannelCount is NULL.
**/
EFI_STATUS SunxiPwmGetChannelCount(
  UINT32 *OutChannelCount
);

/**
 Configures GPIO for PWM, must be called prior to calling SunxiPwmConfigureChannel
 or PWM may not work.

 @param  Channel                 Channel to prepare, SunxiPwmPrepareChannel maps
                                 channel to corresponding GPIO pin and sets pwm
                                 function on that pin.

 @retval EFI_NOT_FOUND           No such channel.
**/
EFI_STATUS SunxiPwmPrepareChannel(UINT32 Channel);

/**
 Initializes PWM channel, configures PWM frequency, polarity, active period,
 GPIO pin.

 @param  Channel                  Channel to configure, use SunxiPwmGetChannelCount to get
                                  number of supported channels.
 @param  Frequency                PWM counter frequency, must be one of SUNXI_PWM_FREQ_*
                                  constants or zero. If zero then EntirePeriod,
                                  ActivePeriod and Flags are ignored and PWM is
                                  disabled.
 @param  EntirePeriod             Period duration in cycles.
 @param  ActivePeriod             For how many cycles output should be active.
 @param  Flags                    Bitwise or of SUNXI_PWM_* constants or 0.

 @retval EFI_NOT_FOUND            No such channel.
 @retval EFI_INVALID_PARAMETER    One of parameters is invalid: unsupported Frequency,
                                  too big EntirePeriod or ActivePeriod, ActivePeriod
                                  bigger than EntirePeriod or Flags contains unknown
                                  flag.
**/
EFI_STATUS SunxiPwmConfigureChannel(
  UINT32 Channel,
  UINT32 Frequency,
  UINT32 EntirePeriod,
  UINT32 ActivePeriod,
  UINT32 Flags
);
