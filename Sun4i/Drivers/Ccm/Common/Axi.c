#include "Driver.h"
#include "Pll.h"

#include <Library/IoLib.h>
#include <Sunxi/HW/ClockSun4i.h>

EFI_STATUS
EFIAPI
CcmGetAxiFrequency(
  IN  CONST CCM_PLL_DESC *Desc,
  OUT UINT32 *Hz
  )
{
  EFI_STATUS Status;
  UINT32 Reg;
  UINT32 Div;
  UINT32 InputFreq;

  if (Hz == NULL)
    return EFI_INVALID_PARAMETER;

  Status = CcmGetCpuFrequency(NULL, &InputFreq);
  if (EFI_ERROR(Status))
    return Status;

  Reg = MmioRead32(CCM_BASE + CCM_OFFSET_CPU_AHB_APB0_CFG);
  Div = ((Reg & AXI_DIV_MASK) >> AXI_DIV_SHIFT) + 1;

  *Hz = InputFreq / Div;

  return EFI_SUCCESS;
}