#include "Driver.h"
#include "Pll.h"

#include <Library/IoLib.h>
#include <Sunxi/HW/ClockSun4i.h>

EFI_STATUS
EFIAPI
CcmGetAhbFrequency(
  IN  CONST CCM_PLL_DESC *Desc,
  OUT UINT32 *Hz
  )
{
  EFI_STATUS Status;
  UINT32 Input;
  UINT32 Reg;
  UINT32 Div;

  if (Hz == NULL)
    return EFI_INVALID_PARAMETER;

  Status = CcmGetAxiFrequency(NULL, &Input);
  if (EFI_ERROR(Status))
    return Status;

  Reg = MmioRead32(CCM_BASE + CCM_OFFSET_CPU_AHB_APB0_CFG);
  Div = 1u << ((Reg & AHB_DIV_MASK) >> AHB_DIV_SHIFT);

  *Hz = Input / Div;

  return EFI_SUCCESS;
}
