[Defines]
  PLATFORM_NAME                  = SunxiPlatformPkg
  PLATFORM_GUID                  = a78dbe5b-704c-4699-9c37-eb5c108f1c73
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x0001001A
  OUTPUT_DIRECTORY               = Build/XW711
  SUPPORTED_ARCHITECTURES        = ARM
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = SunxiPlatformPkg/XW711.fdf

  [Defines]
  DEFINE FD_SIZE          = 0x300000
  DEFINE FD_NUM_BLOCKS    = 0x300

  DEFINE DEBUG_PRINT_ERROR_LEVEL  = 0x8000004F
  #DEFINE DEBUG_PRINT_ERROR_LEVEL = 0x800001CF
  DEFINE TTY_TERMINAL             = TRUE

!include SunxiPlatformPkg/Conf/Sunxi.dsc.inc
!include SunxiPlatformPkg/SPL/Conf/XW711.dsc.inc

!include SunxiPlatformPkg/Conf/Sun5i/A13.dsc.inc

[Components]
  SunxiPlatformPkg/Board/XW711/I2cEnumerate/I2cEnumeratePei.inf
  SunxiPlatformPkg/Board/XW711/I2cEnumerate/I2cEnumerateDxe.inf

[PcdsFixedAtBuild]
  gAxp209TokenSpaceGuid.Dcdc2Voltage | 1400
  gAxp209TokenSpaceGuid.Dcdc3Voltage | 1200
  gAxp209TokenSpaceGuid.Ldo2Voltage  | 3000
  gAxp209TokenSpaceGuid.Ldo3Voltage  | 0
  gAxp209TokenSpaceGuid.Ldo4Voltage  | 0

  gSunxiTokenSpaceGuid.FramebufferReserved     | 1536000

  gSunxiTokenSpaceGuid.LcdFlags                | 0x00000003
  gSunxiTokenSpaceGuid.LcdInterface            | 0
  gSunxiTokenSpaceGuid.LcdBacklightEnablePin   | 1
  gSunxiTokenSpaceGuid.LcdPowerPin             | 0
  gSunxiTokenSpaceGuid.LcdHorizontalResolution | 800
  gSunxiTokenSpaceGuid.LcdVerticalResolution   | 480
  gSunxiTokenSpaceGuid.LcdRefreshRate          | 60
  gSunxiTokenSpaceGuid.LcdPixclock             | 39683
  gSunxiTokenSpaceGuid.LcdPixclockKHz          | 33000
  gSunxiTokenSpaceGuid.LcdHBackPorch           | 87
  gSunxiTokenSpaceGuid.LcdHFrontPorch          | 40
  gSunxiTokenSpaceGuid.LcdVBackPorch           | 31
  gSunxiTokenSpaceGuid.LcdVFrontPorch          | 13
  gSunxiTokenSpaceGuid.LcdHSync                | 1
  gSunxiTokenSpaceGuid.LcdVSync                | 1
  gSunxiTokenSpaceGuid.LcdFlags                | 3
  gSunxiTokenSpaceGuid.LcdDepth                | 18
  gSunxiTokenSpaceGuid.LcdPwmChannel           | 0
  gSunxiTokenSpaceGuid.LcdPwmFreq              | 24000000
  gSunxiTokenSpaceGuid.LcdPwmEntirePeriod      | 1200
  gSunxiTokenSpaceGuid.LcdPwmActivePeriod      | 960
