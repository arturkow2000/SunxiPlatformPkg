[PcdsFixedAtBuild]
!if $(UART_ON_MMC) != TRUE
  # UART0 is multiplexed with MMC, if we use UART, we can't use MMC.
  gSunxiTokenSpaceGuid.Mmc0Base     | 0x01c0f000
!endif
  # MMC1 seems unused on all A13 boards.
  # gSunxiTokenSpaceGuid.Mmc1Base     | 0x01c10000
  # FIXME: eMMC support is broken
  #gSunxiTokenSpaceGuid.Mmc2Base     | 0x01c11000

  gSunxiTokenSpaceGuid.CcmBase      | 0x01c20000
!if $(UART_ON_MMC) == TRUE
  gSunxiTokenSpaceGuid.UartBase     | 0x01c28000
!else
  gSunxiTokenSpaceGuid.UartBase     | 0x01c28400
!endif
  gSunxiTokenSpaceGuid.PcdGpioBase  | 0x01c20800
  gSunxiTokenSpaceGuid.TimerBase    | 0x01c20c00
  gSunxiTokenSpaceGuid.WatchdogBase | 0x01c20c90

  gSunxiTokenSpaceGuid.I2c0Base     | 0x01c2ac00
  gSunxiTokenSpaceGuid.I2c1Base     | 0x01c2b000
  gSunxiTokenSpaceGuid.I2c2Base     | 0x01c2b400

[Components.common]
  SunxiPlatformPkg/Sun4i/Drivers/Ccm/A13CcmDxe.inf
  SunxiPlatformPkg/Sun4i/Drivers/TimerDxe/TimerDxe.inf
  SunxiPlatformPkg/Drivers/Usb/Phy/A13PhyPei.inf
  SunxiPlatformPkg/Drivers/Usb/Otg/A13OtgPei.inf

[Components]
  #
  # PEI
  #
  SunxiPlatformPkg/Sun4i/Drivers/Ccm/A13CcmPei.inf

[LibraryClasses]
  SunxiPwmLib|SunxiPlatformPkg/Library/SunxiPwmLib/PwmSun5i.inf
