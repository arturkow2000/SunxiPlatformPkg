/**
 SPDX-License-Identifier: GPL-2.0+
 
 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on U-Boot driver
 (C) Copyright 2013-2014 Luc Verhaegen <libv@skynet.be>
 (C) Copyright 2014-2015 Hans de Goede <hdegoede@redhat.com>

**/

#include "Driver.h"
#include "Lcd.h"
#include <Library/SunxiGpioLib.h>

EFI_STATUS SunxiLcdPanelPowerControl(SUNXI_DISPLAY_DRIVER *Driver, BOOLEAN PowerOn) {
  UINT32 Pin = FixedPcdGet32(LcdPowerPin);

  return Driver->Pmic->GpioOutput(Driver->Pmic, Pin, !!PowerOn);
}

EFI_STATUS SunxiLcdBacklightControl(SUNXI_DISPLAY_DRIVER *Driver, BOOLEAN PowerOn) {
  EFI_STATUS Status;
  UINT32 Pin = FixedPcdGet32(LcdBacklightEnablePin);

  Status = Driver->Pmic->GpioOutput(Driver->Pmic, Pin, !!PowerOn);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  // FIXME: hardcoded PWM pin for sun5i
  // TODO: should use PWM library for doing this
  // TODO: implement an actual PWM
  SunxiGpioSetLevel(SUNXI_GPIO_PIN_PB(2), !(FixedPcdGet32(LcdFlags) & LCD_PWM_POLARITY_LOW));
  SunxiGpioConfigureAsOutput(SUNXI_GPIO_PIN_PB(2));
  return EFI_SUCCESS;
}

EFI_STATUS SunxiLcdInit(SUNXI_DISPLAY_DRIVER *Driver) {
  EFI_STATUS Status;
  UINT32 Gate;

  Status = Driver->SunxiCcmProtocol->GetGate(Driver->SunxiCcmProtocol, L"ahb1-lcd", &Gate);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = Driver->SunxiCcmProtocol->GateUnmask(Driver->SunxiCcmProtocol, Gate);
  ASSERT_EFI_ERROR(Status);

  MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_CTRL, 0);
  MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_INT0, 0);

  MmioAnd32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_DCLK, ~SUNXI_LCDC_TCON0_DCLK_ENABLE);

  MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_IO_TRISTATE, 0xffffffff);
  MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON1_IO_TRISTATE, 0xffffffff);

  return Status;
}

STATIC UINT32 GetClkDelay (
  IN SUNXI_VIDEO_MODE *Mode,
  IN UINT32 Tcon
) {
  UINT32 Delay;
  
  Delay = Mode->VFrontPorch + Mode->VSync + Mode->VBackPorch;
  if (Mode->Flags & VIDEO_MODE_INTERLACED)
    Delay /= 2;
  if (Tcon == 1)
    Delay -= 2;

  return (Delay > 30) ? 30 : Delay;
}

EFI_STATUS SunxiLcdSetMode(SUNXI_DISPLAY_DRIVER *Driver, SUNXI_VIDEO_MODE *Mode)
{
  EFI_STATUS Status;
  UINT32 ClkDiv, ClkDouble, Bp, ClkDelay, Total, Value;
  UINT32 i;
  UINT32 DclkPhase = 1;

  Status = SunxiLcdSetClock(Driver, 0, Mode->PixclockKHz, &ClkDiv, &ClkDouble);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  ClkDelay = GetClkDelay(Mode, 0);
  MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_CTRL, SUNXI_LCDC_TCON0_CTRL_ENABLE | SUNXI_LCDC_TCON0_CTRL_CLK_DELAY(ClkDelay));
  MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_DCLK, SUNXI_LCDC_TCON0_DCLK_ENABLE | SUNXI_LCDC_TCON0_DCLK_DIV(ClkDiv));
  MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_TIMING_ACTIVE, SUNXI_LCDC_X(Mode->Width) | SUNXI_LCDC_Y(Mode->Height));

  Bp = Mode->HSync + Mode->HBackPorch;
  Total = Mode->Width + Mode->HFrontPorch + Bp;
  MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_TIMING_H, SUNXI_LCDC_TCON0_TIMING_H_TOTAL(Total) | SUNXI_LCDC_TCON0_TIMING_H_BP(Bp));

  Bp = Mode->VSync + Mode->VBackPorch;
  Total = Mode->Height + Mode->VFrontPorch + Bp;
  MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_TIMING_V, SUNXI_LCDC_TCON0_TIMING_V_TOTAL(Total) | SUNXI_LCDC_TCON0_TIMING_V_BP(Bp));

  if (FixedPcdGet32(LcdInterface) == LCD_INTERFACE_PARALLEL) {
    MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_TIMING_SYNC, SUNXI_LCDC_X(Mode->HSync) | SUNXI_LCDC_Y(Mode->VSync));

    MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_HV_INTF, 0);
    MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_CPU_INTF, 0);
  } else if (FixedPcdGet32(LcdInterface) == LCD_INTERFACE_LVDS) {
    Value = (Mode->Depth == 18) ? 1 : 0;
    MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_LVDS_INTF, SUNXI_LCDC_TCON0_LVDS_INTF_BITWIDTH(Value) | SUNXI_LCDC_TCON0_LVDS_CLK_SEL_TCON0);
  }

  if (Mode->Depth == 18 || Mode->Depth == 16) {
    for (i = 0; i < 6; i++) {
      MmioWrite32(
        SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_FRM_SEED_0 + i * 4,
        0x11111111
      );
    }

    MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_FRM_TABLE_0, 0x01010000);
    MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_FRM_TABLE_1, 0x15151111);
    MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_FRM_TABLE_2, 0x57575555);
    MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_FRM_TABLE_3, 0x7f7f7777);

    Value = Mode->Depth == 18 ? SUNXI_LCDC_TCON0_FRM_CTRL_RGB666 : SUNXI_LCDC_TCON0_FRM_CTRL_RGB565;
    MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_FRM_CTRL, Value);
  }

  Value = SUNXI_LCDC_TCON0_IO_POL_DCLK_PHASE(DclkPhase);
  if (!(Mode->Flags & VIDEO_MODE_HSYNC_HIGH))
    Value |= SUNXI_LCDC_TCON_HSYNC_MASK;
  if (!(Mode->Flags & VIDEO_MODE_HSYNC_HIGH))
    Value |= SUNXI_LCDC_TCON_VSYNC_MASK;

  MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_IO_POLARITY, Value);
  MmioWrite32(SUNXI_LCD0_BASE + SUNXI_LCD_TCON0_IO_TRISTATE, 0);

  return EFI_SUCCESS;
}

EFI_STATUS SunxiLcdEnable(SUNXI_DISPLAY_DRIVER *Driver) {
  MmioOr32(SUNXI_LCD0_BASE + SUNXI_LCD_CTRL, SUNXI_LCDC_CTRL_TCON_ENABLE);

  return EFI_SUCCESS;
}

