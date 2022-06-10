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
  SunxiPlatformPkg/Drivers/Usb/Otg/A13OtgDxe.inf

  SunxiPlatformPkg/Drivers/Usb/ExperimentalGadgetDriver/Driver.inf

[Components]
  #
  # PEI
  #
  SunxiPlatformPkg/Sun4i/Drivers/Ccm/A13CcmPei.inf

[LibraryClasses]
  SunxiPwmLib|SunxiPlatformPkg/Library/SunxiPwmLib/PwmSun5i.inf
  SunxiUsbPhyLib|SunxiPlatformPkg/Library/SunxiUsbPhyLib/A13.inf

[LibraryClasses.common.PEIM]
  SunxiCcmLib|SunxiPlatformPkg/Library/SunxiCcmLib/SunxiCcmLibPei.inf

[LibraryClasses.common.DXE_DRIVER]
  SunxiCcmLib|SunxiPlatformPkg/Library/SunxiCcmLib/SunxiCcmLibDxe.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  SunxiCcmLib|SunxiPlatformPkg/Library/SunxiCcmLib/SunxiCcmLibDxe.inf

[LibraryClasses.common.UEFI_DRIVER]
  SunxiCcmLib|SunxiPlatformPkg/Library/SunxiCcmLib/SunxiCcmLibDxe.inf