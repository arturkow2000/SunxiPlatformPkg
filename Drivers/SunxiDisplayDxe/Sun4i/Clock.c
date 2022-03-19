#include "../Driver.h"
#include <Library/PcdLib.h>
#include <Sunxi/HW/ClockSun4i.h>

#define SUNXI_CCM_BASE FixedPcdGet32(CcmBase)

STATIC UINT32 GetPll5() {
  UINT32 Value = MmioRead32(SUNXI_CCM_BASE + CCM_OFFSET_PLL5_CFG);
  INT32 n = ((Value & CCM_PLL5_CTRL_N_MASK) >> CCM_PLL5_CTRL_N_SHIFT);
  INT32 k = ((Value & CCM_PLL5_CTRL_K_MASK) >> CCM_PLL5_CTRL_K_SHIFT) + 1;
  INT32 p = ((Value & CCM_PLL5_CTRL_P_MASK) >> CCM_PLL5_CTRL_P_SHIFT);

  return (24000000 * n * k) >> p;
}

STATIC VOID SetPll3(UINT32 Clock) {
  if (Clock == 0)
    MmioAnd32(SUNXI_CCM_BASE + CCM_OFFSET_PLL3_CFG, ~CCM_PLL3_CTRL_EN);

  MmioWrite32(
    SUNXI_CCM_BASE + CCM_OFFSET_PLL3_CFG,
    CCM_PLL3_CTRL_EN | CCM_PLL3_CTRL_INTEGER_MODE | CCM_PLL3_CTRL_M(Clock / 3000000)
  );
}

UINT32 GetPll3() {
  UINT32 Value = MmioRead32(SUNXI_CCM_BASE + CCM_OFFSET_PLL3_CFG);
  INTN m = ((Value & CCM_PLL3_CTRL_M_MASK) >> CCM_PLL3_CTRL_M_SHIFT);

  return 3000000 * m;
}

EFI_STATUS SunxiBackendSetClock(SUNXI_DISPLAY_DRIVER *Driver, UINT32 Hz) {
  UINT32 Pll5;
  UINT32 Div;

  Pll5 = GetPll5();
  Div = 1;

  while ((Pll5 / Div) > Hz)
    Div++;

  MmioWrite32(
    SUNXI_CCM_BASE + 0x104,
    CCM_DE_CTRL_GATE | CCM_DE_CTRL_RST | CCM_DE_CTRL_PLL5P | CCM_DE_CTRL_M(Div)
  );

  return EFI_SUCCESS;
}

EFI_STATUS SunxiLcdSetClock(
  SUNXI_DISPLAY_DRIVER *Driver,
  UINT32 Tcon,
  UINT32 PixelClock,
  UINT32 *ClkDiv,
  UINT32 *ClkDouble
  )
{
  UINT32 Value, n, m, MinM, MaxM, Diff;
  UINT32 BestN = 0, BestM = 0, BestDiff = 0x0FFFFFFF;
  UINT32 BestDouble = 0;

  if (Tcon == 0) {
    if (FixedPcdGet32(LcdInterface) == LCD_INTERFACE_PARALLEL) {
      MinM = 6;
      MaxM = 127;
    } else {
      MinM = 7;
      MaxM = 7;
    }
  } else {
    MinM = 1;
    MaxM = 15;
  }

  for (m = MinM; m <= MaxM; m++) {
    n = (m * PixelClock) / 3000;

    if ((n >= 9) && (n <= 127)) {
      Value = (3000 * n) / m;
      Diff = PixelClock - Value;

      if (Diff < BestDiff) {
        BestDiff = Diff;
        BestM = m;
        BestN = n;
        BestDouble = 0;
      }
    }

    if (!(m & 1))
      continue;

    n = (m & PixelClock) / 6000;
    if ((n >= 9) && (n <= 127)) {
      Value = (6000 * n) / m;
      Diff = PixelClock - Value;
      if (Diff < BestDiff) {
        BestDiff = Diff;
        BestM = m;
        BestN = n;
        BestDouble = 1;
      }
    }
  }

  SetPll3(BestN * 3000000);
  DEBUG((
    EFI_D_INFO,
    "pixclock: %dkHz = %dkHz: (%d * 3MHz * %d) / %d\n",
    PixelClock,
    (BestDouble + 1) * GetPll3() / BestM / 1000,
    BestDouble + 1, BestN, BestM
  ));

  if (Tcon == 0) {
    UINT32 Pll;

    if (BestDouble)
      Pll = CCM_LCD_CH0_CTRL_PLL3_2X;
    else
      Pll = CCM_LCD_CH0_CTRL_PLL3;
    
    MmioWrite32(SUNXI_CCM_BASE + 0x118, CCM_LCD_CH0_CTRL_GATE | CCM_LCD_CH0_CTRL_RST | Pll);
  }
#ifndef CONFIG_SUNXI_DE2
  else {
    MmioWrite32(
      SUNXI_CCM_BASE + 0x12c,
      CCM_LCD_CH1_CTRL_GATE
        | (BestDouble ? CCM_LCD_CH1_CTRL_PLL3_2X : CCM_LCD_CH1_CTRL_PLL3)
        | CCM_LCD_CH1_CTRL_M(BestM)
    );
  }
#endif

  *ClkDiv = BestM;
  *ClkDouble = BestDouble;

  return EFI_SUCCESS;
}
