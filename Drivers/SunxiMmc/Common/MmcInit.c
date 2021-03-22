/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski <arturkow2000@gmail.com>

 Based on code from U-Boot
 Copyright 2008, Freescale Semiconductor, Inc
 Copyright 2020 NXP
 Andy Fleming
**/

#include <Uefi.h>

#include <Library/DebugLib.h>
#include <Library/TimerLib.h>

#include "Driver.h"
#include "MmcDef.h"

EFI_STATUS MmcSelectModeAndWidth(IN SUNXI_MMC *Mmc);

STATIC
EFI_STATUS
GetSdOpCond(
  IN SUNXI_MMC *Mmc,
  OUT UINT32 *Ocr OPTIONAL
  )
{
  EFI_STATUS Status;
  INTN i;
  UINT32 Resp;

  for (i = 0; i < 100; i++) {
    Status = SunxiMmcExecuteCommand(
      Mmc,
      MMC_CMD_APP_CMD,
      0,
      MMC_RSP_R1,
      NULL
    );
    if (EFI_ERROR(Status))
      return Status;
    
    Status = SunxiMmcExecuteCommand(
      Mmc,
      SD_ACMD_SEND_OP_COND,
      (Mmc->Config->SupportedVoltagesOcr & 0xff8000)
       | (Mmc->State.CardInfo.Version == SD_VERSION_2 ? OCR_HCS : 0),
      MMC_RSP_R3,
      &Resp
    );
    if (EFI_ERROR(Status))
      return Status;
    
    if (Resp & OCR_BUSY)
    {
      if (Ocr)
        *Ocr = Resp;

      Mmc->State.CardInfo.HighCapacity = !!(Resp & OCR_HCS);
      if (Mmc->State.CardInfo.Version != SD_VERSION_2)
        Mmc->State.CardInfo.Version = SD_VERSION_1_0;

      return EFI_SUCCESS;
    }

    MicroSecondDelay(1000);
  }

  DEBUG((EFI_D_ERROR, "SD_ACMD_SEND_OP_COND timeout after %u attempts\n", i));

  return EFI_TIMEOUT;
}

STATIC
EFI_STATUS
MmcSendOpCondIter(
  IN SUNXI_MMC *Mmc,
  IN BOOLEAN UseArg,
  IN OUT UINT32 *Ocr
  )
{
  EFI_STATUS Status;
  UINT32 Resp;

  Status = SunxiMmcExecuteCommand(
    Mmc,
    MMC_CMD_SEND_OP_COND,
    OCR_HCS |
      (Mmc->Config->SupportedVoltagesOcr & (*Ocr & OCR_VOLTAGE_MASK)) | (*Ocr & OCR_ACCESS_MODE),
    MMC_RSP_R3,
    &Resp
  );
  if (EFI_ERROR(Status))
    return Status;

  *Ocr = Resp;

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
MmcSendOpCond(
  IN SUNXI_MMC *Mmc
  )
{
  INT32 Timeout;
  INT32 i;
  EFI_STATUS Status;
  UINT32 Ocr;

  Timeout = 1000;
  Ocr = 0;

  SunxiMmcExecuteCommand(
    Mmc,
    MMC_CMD_GO_IDLE_STATE,
    0,
    MMC_RSP_NONE,
    NULL
  );

  for (i = 0; i < Timeout; i++) {
    Status = MmcSendOpCondIter(Mmc, i != 0, &Ocr);
    if (EFI_ERROR(Status))
      return Status;

    DEBUG((EFI_D_ERROR, "OCR = 0x%x\n", Ocr));

    if (Ocr & OCR_BUSY)
      break;
    
    MicroSecondDelay(100);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SunxiMmcGetOpCond(
  IN SUNXI_MMC *Mmc
  )
{
  EFI_STATUS Status;
  UINT32 Ocr;
  UINT32 Resp;

  Status = SunxiMmcSetIos(Mmc, Mmc->Config->MinFreq, 1);
  if (EFI_ERROR(Status))
    goto Exit;

  Status = SunxiMmcExecuteCommand(
    Mmc,
    MMC_CMD_GO_IDLE_STATE,
    0,
    MMC_RSP_NONE,
    NULL
  );
  if (EFI_ERROR(Status))
    goto Exit;

  Status = SunxiMmcExecuteCommand(
    Mmc,
    SD_CMD_SEND_IF_COND,
    ((Mmc->Config->SupportedVoltagesOcr & 0xff8000) != 0) << 8 | 0xaa,
    MMC_RSP_R7,
    &Resp
  );
  if (Status == EFI_SUCCESS) {
    if ((Resp & 0xff) == 0xaa)
      Mmc->State.CardInfo.Version = SD_VERSION_2;
  }
  else // ignore error
    Status = EFI_SUCCESS;

  Status = GetSdOpCond(Mmc, &Ocr);
  if (Status == EFI_TIMEOUT)
    goto TryMmcInit;

  if (EFI_ERROR(Status))
    goto Exit;

  Exit:
  return Status;

  TryMmcInit:
  Status = MmcSendOpCond(Mmc);
  goto Exit;
}

STATIC
EFI_STATUS
SunxiMmcStart(
  IN SUNXI_MMC *Mmc,
  IN UINT32 *Cid
);

EFI_STATUS
SunxiMmcInitMmc(
  IN SUNXI_MMC *Mmc
  )
{
  EFI_STATUS Status;
  UINT32 Cid[4];

  // Reset card info
  // TODO: move this into separate function
  Mmc->State.CardInfo.Version = MMC_VERSION_UNKNOWN;
  Mmc->State.CardInfo.HighCapacity = FALSE;
  Mmc->State.CardInfo.Rca = 0;
  Mmc->State.CardInfo.Csd[0] = 0;
  Mmc->State.CardInfo.Csd[1] = 0;
  Mmc->State.CardInfo.Csd[2] = 0;
  Mmc->State.CardInfo.Csd[3] = 0;
  Mmc->State.CardInfo.LegacyFreq = 0;

  // Initialize card
  Status = SunxiMmcGetOpCond(Mmc);
  if (EFI_ERROR(Status))
    goto Exit;

  Status = SunxiMmcExecuteCommand(
    Mmc,
    MMC_CMD_ALL_SEND_CID,
    0,
    MMC_RSP_R2,
    Cid
  );
  if (EFI_ERROR(Status))
    goto Exit;

  DEBUG((EFI_D_INFO, "cid: 0x%x 0x%x 0x%x 0x%x\n", Cid[0], Cid[1], Cid[2], Cid[3]));

  Status = SunxiMmcStart(Mmc, Cid);
  if (EFI_ERROR(Status))
    goto Exit;

  Exit:
  return Status;
}

/* frequency bases */
/* divided by 10 to be nice to platforms without floating point */
STATIC CONST INT32 mFBase[] = {
  10000,
  100000,
  1000000,
  10000000,
};

/* Multiplier values for TRAN_SPEED.  Multiplied by 10 to be nice
 * to platforms without floating point.
 */
STATIC CONST UINT8 mMultipliers[] = {
  0,      /* reserved */
  10,
  12,
  13,
  15,
  20,
  25,
  30,
  35,
  40,
  45,
  50,
  55,
  60,
  70,
  80,
};

STATIC
EFI_STATUS
SunxiMmcStart(
  IN SUNXI_MMC *Mmc,
  IN UINT32 *Cid
  )
{
  EFI_STATUS Status;
  UINT32 Resp;
  UINT32 Version;
  UINT32 Freq;
  UINT32 Mult;

  Status = SunxiMmcExecuteCommand(
    Mmc,
    MMC_CMD_SET_RELATIVE_ADDR,
    1 << 16,
    MMC_RSP_R6,
    &Resp
  );
  if (EFI_ERROR(Status))
    goto Exit;

  if (Mmc->State.CardInfo.Version & SD_VERSION_SD) {
    Mmc->State.CardInfo.Rca = Resp >> 16;
  } else  {
    Mmc->State.CardInfo.Rca = 1;
  }

  Status = SunxiMmcExecuteCommand(
    Mmc,
    MMC_CMD_SEND_CSD,
    (UINT32)Mmc->State.CardInfo.Rca << 16,
    MMC_RSP_R2,
    Mmc->State.CardInfo.Csd
  );
  if (EFI_ERROR(Status))
    goto Exit;

  DEBUG((
    EFI_D_INFO,
    "csd: 0x%x 0x%x 0x%x 0x%x\n",
    Mmc->State.CardInfo.Csd[0],
    Mmc->State.CardInfo.Csd[1],
    Mmc->State.CardInfo.Csd[2],
    Mmc->State.CardInfo.Csd[3]
  ));

  if (Mmc->State.CardInfo.Version == MMC_VERSION_UNKNOWN) {
    Version = (Mmc->State.CardInfo.Csd[0] >> 26) & 0xf;

    switch (Version) {
    case 0:
      Mmc->State.CardInfo.Version = MMC_VERSION_1_2;
      break;
    case 1:
      Mmc->State.CardInfo.Version = MMC_VERSION_1_4;
      break;
    case 2:
      Mmc->State.CardInfo.Version = MMC_VERSION_2_2;
      break;
    case 3:
      Mmc->State.CardInfo.Version = MMC_VERSION_3;
      break;
    case 4:
      Mmc->State.CardInfo.Version = MMC_VERSION_4;
      break;
    default:
      Mmc->State.CardInfo.Version = MMC_VERSION_1_2;
      break;
    }
  }

  Freq = mFBase[Mmc->State.CardInfo.Csd[0] & 0x7];
  Mult = mMultipliers[(Mmc->State.CardInfo.Csd[0] >> 3) & 0xf];

  Mmc->State.CardInfo.LegacyFreq = Freq * Mult;

  DEBUG((EFI_D_INFO, "Selecting frequency: %u\n", Mmc->State.CardInfo.LegacyFreq));
  Status = SunxiMmcSetIos(
    Mmc,
    Mmc->State.CardInfo.LegacyFreq,
    1
  );
  if (EFI_ERROR(Status))
    goto Exit;

  Status = SunxiMmcExecuteCommand(
    Mmc,
    MMC_CMD_SELECT_CARD,
    Mmc->State.CardInfo.Rca << 16,
    MMC_RSP_R1,
    NULL
  );
  if (EFI_ERROR(Status))
    goto Exit;

  MmcSelectModeAndWidth(Mmc);

  Exit:
  return Status;
}