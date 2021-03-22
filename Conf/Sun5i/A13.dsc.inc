[PcdsFixedAtBuild]
  gSunxiTokenSpaceGuid.Mmc0Base     | 0x01c0f000
  # MMC1 seems unused on all A13 boards.
  # gSunxiTokenSpaceGuid.Mmc1Base     | 0x01c10000
  # FIXME: eMMC support is broken
  #gSunxiTokenSpaceGuid.Mmc2Base     | 0x01c11000

  gSunxiTokenSpaceGuid.CcmBase      | 0x01c20000
  gSunxiTokenSpaceGuid.UartBase     | 0x01c28400
  gSunxiTokenSpaceGuid.PcdGpioBase  | 0x01c20800
  gSunxiTokenSpaceGuid.TimerBase    | 0x01c20c00
  gSunxiTokenSpaceGuid.WatchdogBase | 0x01c20c90

[Components.common]
  SunxiPlatformPkg/Sun4i/Drivers/Ccm/A13CcmDxe.inf
  SunxiPlatformPkg/Sun4i/Drivers/TimerDxe/TimerDxe.inf