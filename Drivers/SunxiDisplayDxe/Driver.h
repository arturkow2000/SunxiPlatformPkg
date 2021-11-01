/**
 SPDX-License-Identifier: GPL-2.0+
 
 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on U-Boot driver
 (C) Copyright 2013-2014 Luc Verhaegen <libv@skynet.be>
 (C) Copyright 2014-2015 Hans de Goede <hdegoede@redhat.com>

**/

#pragma once

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>

#include <Protocol/GraphicsOutput.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SunxiCcm.h>
#include <Protocol/Pmic.h>
#include <Protocol/Cpu.h>

#define SUNXI_BE0_BASE 0x01e60000
#define SUNXI_LCD0_BASE 0x01c0C000

#define LCD_PRESENT (1 << 0)
#define LCD_PWM_POLARITY_LOW (1 << 1)

#define LCD_INTERFACE_PARALLEL 0
#define LCD_INTERFACE_LVDS 1

#define VIDEO_MODE_HSYNC_HIGH (1 << 0)
#define VIDEO_MODE_VSYNC_HIGH (1 << 1)
#define VIDEO_MODE_INTERLACED (1 << 2)

typedef struct _SUNXI_VIDEO_MODE {
  UINT32 Width;
  UINT32 Height;
  UINT32 RefreshRate;
  UINT32 Pixclock;
  UINT32 PixclockKHz;
  UINT32 HBackPorch;
  UINT32 HFrontPorch;
  UINT32 VBackPorch;
  UINT32 VFrontPorch;
  UINT32 HSync;
  UINT32 VSync;
  UINT32 Flags;
  UINT8 Depth;
} SUNXI_VIDEO_MODE;

#define GOP_TO_DISPLAY_DRIVER(Gop)  \
  ((SUNXI_DISPLAY_DRIVER*)Gop)

typedef struct _SUNXI_DISPLAY_DRIVER
{
  EFI_GRAPHICS_OUTPUT_PROTOCOL GraphicsOutputProtocol;
  SUNXI_CCM_PROTOCOL *SunxiCcmProtocol;
  PMIC_PROTOCOL *Pmic;
  EFI_HANDLE GopHandle;
  EFI_DEVICE_PATH_PROTOCOL *GopDevicePath;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE GopMode;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION GopModeInfo;
} SUNXI_DISPLAY_DRIVER;

EFI_STATUS SunxiDisplayInit(SUNXI_DISPLAY_DRIVER *Driver);
EFI_STATUS SunxiDisplayInitGpio(SUNXI_DISPLAY_DRIVER *Driver);
EFI_STATUS SunxiDisplayInitGop(SUNXI_DISPLAY_DRIVER *Driver, VOID *Framebuffer, SUNXI_VIDEO_MODE *Mode);

EFI_STATUS SunxiBackendInit(SUNXI_DISPLAY_DRIVER *Driver);
EFI_STATUS SunxiBackendSetClock(SUNXI_DISPLAY_DRIVER *Driver, UINT32 Hz);
EFI_STATUS SunxiBackendSetMode(SUNXI_DISPLAY_DRIVER *Driver, SUNXI_VIDEO_MODE *Mode, UINT32 FramebufferBase);
EFI_STATUS SunxiBackendEnable(SUNXI_DISPLAY_DRIVER *Driver);

EFI_STATUS SunxiLcdInit(SUNXI_DISPLAY_DRIVER *Driver);
EFI_STATUS SunxiLcdSetMode(SUNXI_DISPLAY_DRIVER *Driver, SUNXI_VIDEO_MODE *Mode);
EFI_STATUS SunxiLcdEnable(SUNXI_DISPLAY_DRIVER *Driver);
EFI_STATUS SunxiLcdSetClock(
  SUNXI_DISPLAY_DRIVER *Driver,
  UINT32 Tcon,
  UINT32 PixelClock,
  UINT32 *ClkDiv,
  UINT32 *ClkDouble
);
EFI_STATUS SunxiLcdPanelPowerControl(SUNXI_DISPLAY_DRIVER *Driver, BOOLEAN PowerOn);
EFI_STATUS SunxiLcdBacklightControl(SUNXI_DISPLAY_DRIVER *Driver, BOOLEAN PowerOn);
