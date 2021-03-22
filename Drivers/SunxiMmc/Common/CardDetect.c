#include <Uefi.h>

#include "Driver.h"

EFI_STATUS SunxiMmcDetectCard(
  IN SUNXI_MMC *Mmc,
  OUT BOOLEAN *IsInserted,
  OUT UINT32 *CardType OPTIONAL
  )
{
  if (!IsInserted)
    return EFI_INVALID_PARAMETER;
  
  // TODO: do real card detection
  *IsInserted = TRUE;

  if (CardType) {
    if (Mmc->Config->IsInternal)
      *CardType = CARD_TYPE_EMMC;
    else
      *CardType = CARD_TYPE_SD;
  }

  return EFI_SUCCESS;
}