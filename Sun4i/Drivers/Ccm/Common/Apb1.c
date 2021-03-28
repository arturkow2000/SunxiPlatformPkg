#include "Driver.h"
#include "Pll.h"

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Sunxi/HW/ClockSun4i.h>

EFI_STATUS
EFIAPI
CcmGetApb1Frequency(
  IN  CONST CCM_PLL_DESC *Desc,
  OUT UINT32 *Hz
  )
{
  EFI_STATUS Status;
  UINT32 Reg;
  UINT32 Src;
  UINT32 InputHz;
  UINT32 m;
  UINT32 n;

  if (Hz == NULL)
    return EFI_INVALID_PARAMETER;

  Reg = MmioRead32(CCM_BASE + CCM_OFFSET_APB1_CLK_DIV_CFG);
  Src = (Reg & APB1_CLK_SRC_MASK) >> APB1_CLK_SRC_SHIFT;

  switch (Src) {
  case APB1_CLK_SRC_OSC24M:
    InputHz = 24000000;
    break;
  case APB1_CLK_SRC_LOSC:
    InputHz = 32000;
    break;
  case APB1_CLK_SRC_PLL6:
    Status = CcmGetPll6GetFrequency(NULL, &InputHz);
    if (EFI_ERROR(Status))
      return Status;
    break;
  default:
    ASSERT(0);
    return EFI_DEVICE_ERROR;
  }

  m = (Reg & APB1_CLK_RATE_M_MASK) + 1;
  n = 1 << ((Reg & APB1_CLK_RATE_N_MASK) >> APB1_CLK_RATE_N_SHIFT);
  
  *Hz = InputHz / (m * n);

  return EFI_SUCCESS;
}