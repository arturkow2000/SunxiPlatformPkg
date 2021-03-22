/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski <arturkow2000@gmail.com>

 Based on code from U-Boot
 Copyright 2008, Freescale Semiconductor, Inc
 Copyright 2020 NXP
 Andy Fleming
**/

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Sunxi/HW/Mmc.h>

#include "Driver.h"
#include "MmcDef.h"

EFI_STATUS
SunxiMmcUpdateClock(
  IN SUNXI_MMC *Mmc
  );

EFI_STATUS SunxiMmcTransferDataUsingCpu(
  IN SUNXI_MMC *Mmc,
  IN UINT32 Length,
  IN OUT VOID *Data,
  IN BOOLEAN Write,
  IN UINT64 Timeout
  )
{
  UINT32 Bit;
  UINT32 *DataU32;
  INTN i;
  EFI_STATUS Status;
  UINT32 SavedTimeout;

  Bit = Write ? SUNXI_MMC_STATUS_FIFO_FULL : SUNXI_MMC_STATUS_FIFO_EMPTY;
  DataU32 = Data;
  Status = EFI_TIMEOUT;
  SavedTimeout = Timeout;

  MmioOr32(Mmc->Config->Base + SUNXI_MMC_GCTRL, SUNXI_MMC_GCTRL_ACCESS_BY_AHB);
  
  for (i = 0; i < Length / 4; i++) {
    while (MmioRead32(Mmc->Config->Base + SUNXI_MMC_STATUS) & Bit)
    {
      // TODO: add timeout
      Timeout--;
      if (Timeout == 0) {
        DEBUG((EFI_D_ERROR, "Data transfer timeout\n"));
        goto Exit;
      }
      gBS->Stall(1);
    }

    if (Write)
      MmioWrite32(Mmc->Config->Base + Mmc->Config->FifoRegOffset, DataU32[i]);
    else
      DataU32[i] = MmioRead32(Mmc->Config->Base + Mmc->Config->FifoRegOffset);

    Timeout = SavedTimeout;
  }

  Status = EFI_SUCCESS;

  Exit:
  return Status;
}

#define ENSURE(cond, msg, sta)      \
  if (!(cond)) {                    \
    DEBUG((EFI_D_ERROR, "CMD%02d %s\n", Command, (msg)));    \
    Status = (sta);                 \
    goto Exit;                      \
  }

EFI_STATUS SunxiMmcRintWait(
  IN SUNXI_MMC *Mmc,
  IN UINT32 Bit,
  IN UINT32 Command,
  IN UINT64 Timeout
) {
  EFI_STATUS Status;
  UINT32 Rint;

  Status = EFI_SUCCESS;

  // TODO: add timeout here
  while (Timeout > 0) {
    Rint = MmioRead32(Mmc->Config->Base + SUNXI_MMC_RINT);
    ENSURE(!(Rint & SUNXI_MMC_RINT_RESP_ERROR), L"response error", EFI_DEVICE_ERROR);
    ENSURE(!(Rint & SUNXI_MMC_RINT_RESP_TIMEOUT), L"response timeout", EFI_TIMEOUT);
    ENSURE(!(Rint & SUNXI_MMC_RINT_RESP_CRC_ERROR), L"response CRC error", EFI_DEVICE_ERROR);
    ENSURE(!(Rint & SUNXI_MMC_RINT_DATA_CRC_ERROR), L"data CRC error", EFI_DEVICE_ERROR);
    ENSURE(!(Rint & SUNXI_MMC_RINT_DATA_TIMEOUT), L"data timeout", EFI_TIMEOUT);
    ENSURE(!(Rint & SUNXI_MMC_RINT_VOLTAGE_CHANGE_DONE), L"unexpected voltage change", EFI_DEVICE_ERROR);
    ENSURE(!(Rint & SUNXI_MMC_RINT_FIFO_RUN_ERROR), L"FIFO run error", EFI_DEVICE_ERROR);
    ENSURE(!(Rint & SUNXI_MMC_RINT_HARD_WARE_LOCKED), L"hardware locked", EFI_DEVICE_ERROR);
    ENSURE(!(Rint & SUNXI_MMC_RINT_START_BIT_ERROR), L"start bit error", EFI_DEVICE_ERROR);
    ENSURE(!(Rint & SUNXI_MMC_RINT_END_BIT_ERROR), L"end bit error", EFI_DEVICE_ERROR);

    if (Rint & Bit)
      goto Exit;

    gBS->Stall(1);
    Timeout--;
  }

  DEBUG((EFI_D_ERROR, "SunxiMmcRintWait: timeout 0x%x\n", Bit));
  Status = EFI_TIMEOUT;

  Exit:
  return Status;
}
#undef ENSURE

EFI_STATUS SunxiMmcExecuteCommandEx(
  IN SUNXI_MMC *Mmc,
  IN UINT16 Opcode,
  IN UINT32 Arg,
  IN UINT32 ResponseType,
  IN UINT32 *OutResponse OPTIONAL,
  IN OUT VOID *Data OPTIONAL,
  IN BOOLEAN Write OPTIONAL,
  IN UINT32 DataLength OPTIONAL,
  IN UINT32 BlockLength OPTIONAL
  )
{
  EFI_STATUS Status;
  UINT32 RawCmd;
  UINT32 Resp[4];
  UINT32 RespLength;
  INT32 i;
  UINT32 NumBlocks;
  UINT64 Timeout;

  // FIXME: should accept timeout as argument
  Timeout = 100000;

  if (Mmc == NULL)
    return EFI_INVALID_PARAMETER;

  // TODO: verify Opcode

  if (DataLength > 0) {
    if (!Data)
      return EFI_INVALID_PARAMETER;

    if (DataLength % BlockLength != 0)
      return EFI_INVALID_PARAMETER;
  } else if (Data)
    return EFI_INVALID_PARAMETER;

  Status = EFI_SUCCESS;

  RawCmd = SUNXI_MMC_CMD_START | Opcode;

  if (ResponseType & MMC_RSP_PRESENT) {
    RawCmd |= SUNXI_MMC_CMD_RESP_EXPIRE;

    if (ResponseType & MMC_RSP_136)
      RawCmd |= SUNXI_MMC_CMD_LONG_RESPONSE;

    if (ResponseType & MMC_RSP_CRC)
      RawCmd |= SUNXI_MMC_CMD_CHECK_RESPONSE_CRC;
  }

  if (Opcode == 0)
    RawCmd |= SUNXI_MMC_CMD_SEND_INIT_SEQ;

  if (Data) {
    NumBlocks = DataLength / BlockLength;

    MmioWrite32(Mmc->Config->Base + SUNXI_MMC_BLKSZ, BlockLength);
    MmioWrite32(Mmc->Config->Base + SUNXI_MMC_BYTECNT, DataLength);

    RawCmd |= SUNXI_MMC_CMD_DATA_EXPIRE | SUNXI_MMC_CMD_WAIT_PRE_OVER;
    if (Write)
      RawCmd |= SUNXI_MMC_CMD_WRITE;

    if (NumBlocks > 1)
      RawCmd |= SUNXI_MMC_CMD_AUTO_STOP;
  }

  MmioWrite32(Mmc->Config->Base + SUNXI_MMC_ARG, Arg);
  MmioWrite32(Mmc->Config->Base + SUNXI_MMC_CMD, RawCmd);

  if (Data) {
    Status = SunxiMmcTransferDataUsingCpu(Mmc, DataLength, Data, Write, Timeout);
    if (EFI_ERROR(Status))
      goto Exit;
  }
  
  Status = SunxiMmcRintWait(Mmc, SUNXI_MMC_RINT_COMMAND_DONE, Opcode, Timeout);
  if (EFI_ERROR(Status))
    goto Exit;

  if (Data) {
    Status = SunxiMmcRintWait(
      Mmc,
      NumBlocks > 1 ? SUNXI_MMC_RINT_AUTO_COMMAND_DONE : SUNXI_MMC_RINT_DATA_OVER,
      Opcode,
      Timeout
    );
    if (EFI_ERROR(Status))
      goto Exit;
  }

  if (ResponseType & MMC_RSP_BUSY) {
    // TODO: add timeout
    
    while (MmioRead32(Mmc->Config->Base + SUNXI_MMC_STATUS) & SUNXI_MMC_STATUS_CARD_DATA_BUSY)
    {
      Timeout--;
      if (Timeout == 0) {
        Status = EFI_TIMEOUT;
        goto Exit;
      }
      gBS->Stall(1);
    }
  }

  if (ResponseType & MMC_RSP_136) {
    RespLength = 4;
    Resp[0] = MmioRead32(Mmc->Config->Base + SUNXI_MMC_RESP3);
    Resp[1] = MmioRead32(Mmc->Config->Base + SUNXI_MMC_RESP2);
    Resp[2] = MmioRead32(Mmc->Config->Base + SUNXI_MMC_RESP1);
    Resp[3] = MmioRead32(Mmc->Config->Base + SUNXI_MMC_RESP0);

    DEBUG((EFI_D_INFO, "CMD%02d resp: 0x%x 0x%x 0x%x 0x%x\n", Opcode, Resp[0], Resp[1], Resp[2], Resp[3]));
  } else if (ResponseType & MMC_RSP_PRESENT) {
    RespLength = 1;
    Resp[0] = MmioRead32(Mmc->Config->Base + SUNXI_MMC_RESP0);

    DEBUG((EFI_D_INFO, "CMD%02d resp: 0x%x\n", Opcode, Resp[0]));
  } else {
    RespLength = 0;
    DEBUG((EFI_D_INFO, "CMD%02d OK (no response)\n", Opcode));
  }

  Exit:
  MmioWrite32(Mmc->Config->Base + SUNXI_MMC_RINT, 0xffffffff);
  if (EFI_ERROR(Status)) {
    MmioWrite32(
      Mmc->Config->Base + SUNXI_MMC_GCTRL,
      SUNXI_MMC_GCTRL_SOFT_RESET | SUNXI_MMC_GCTRL_FIFO_RESET | SUNXI_MMC_GCTRL_DMA_RESET
    );
    SunxiMmcUpdateClock(Mmc);
  } else {
    MmioWrite32(
      Mmc->Config->Base + SUNXI_MMC_GCTRL,
      SUNXI_MMC_GCTRL_FIFO_RESET
    );

    if (OutResponse && RespLength > 0) {
      for (i = 0; i < RespLength; i++)
        OutResponse[i] = Resp[i];
    }
  }
  return Status;
}

EFI_STATUS SunxiMmcExecuteCommand(
  IN SUNXI_MMC *Mmc,
  IN UINT16 Opcode,
  IN UINT32 Arg,
  IN UINT32 ResponseType,
  IN UINT32 *OutResponse OPTIONAL
  )
{
  return SunxiMmcExecuteCommandEx(
    Mmc,
    Opcode,
    Arg,
    ResponseType,
    OutResponse,
    NULL,
    FALSE,
    0,
    0
  );
}
