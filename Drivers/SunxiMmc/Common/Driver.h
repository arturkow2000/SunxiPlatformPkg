/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski <arturkow2000@gmail.com>

 Based on code from U-Boot
 Copyright 2008, Freescale Semiconductor, Inc
 Copyright 2020 NXP
 Andy Fleming
**/

#pragma once

typedef struct _SUNXI_GPIO_CONFIG {
  CONST CHAR16 *Pin;
  CONST CHAR16 *Function;
  UINT32 Pull;
  UINT32 DriveStrength;
} SUNXI_GPIO_CONFIG;

typedef struct _SUNXI_MMC_CONFIG {
  CONST SUNXI_GPIO_CONFIG *GpioConfig;
  UINT32 Base;
  UINT32 MaxBusWidth;
  UINT32 MinFreq;
  UINT32 MaxFreq;
  UINT32 FifoRegOffset;
  UINT32 SupportedVoltagesOcr;
  BOOLEAN IsInternal;
  CONST CHAR16 *AhbGate;
  CONST CHAR16 *MmcClock;
} SUNXI_MMC_CONFIG;

typedef struct _SUNXI_MMC_CARD_INFO {
  UINT32 Version;
  BOOLEAN HighCapacity;
  UINT16 Rca;
  UINT32 Csd[4];
  UINT32 LegacyFreq;
} SUNXI_MMC_CARD_INFO;

typedef struct _SUNXI_MMC_STATE {
  UINT32 ClockHz;

  SUNXI_MMC_CARD_INFO CardInfo;
} SUNXI_MMC_STATE;

typedef struct _SUNXI_MMC {
  CONST SUNXI_MMC_CONFIG *Config;
  SUNXI_MMC_STATE State;
} SUNXI_MMC;

EFI_STATUS SunxiMmcInitDriver(
  IN SUNXI_MMC *Mmc
);

EFI_STATUS SunxiMmcInitGpio(
  IN SUNXI_MMC *Mmc
);

/**
 Initialize plugged in MMC/SD card.
**/
EFI_STATUS SunxiMmcInitMmc(
  IN SUNXI_MMC *Mmc
);

EFI_STATUS SunxiMmcResetHw(
  IN SUNXI_MMC *Mmc
);

EFI_STATUS SunxiMmcSetModClock(
  IN SUNXI_MMC *Mmc,
  IN UINT32 Hz
);

/**
 Send command to MMC card.

 @param Mmc                     Pointer to Mmc driver instance.
 @param Opcode                  Command opcode.
 @param Arg                     Command argument.
 @param ResponseType            MMC response type.
 @param OutResponse             Pointer to variable to receive response.
                                Should be 4 32-bit words long for R2 response.
                                For other response types 1 word will be enough.

 @retval EFI_SUCCESS            Command executed.
 @retval EFI_TIMEOUT            Command timed out.
 @retval EFI_DEVICE_ERROR       Any non-timeout device error.
 @retval EFI_INVALID_PARAMETER  Mmc is NULL, Opcode or ResponseType is invalid.
**/
EFI_STATUS SunxiMmcExecuteCommand(
  IN SUNXI_MMC *Mmc,
  IN UINT16 Opcode,
  IN UINT32 Arg,
  IN UINT32 ResponseType,
  IN UINT32 *OutResponse OPTIONAL
);

/**
 Send command to MMC card, with optional data transfer.

 @param Mmc                     Pointer to Mmc driver instance.
 @param Opcode                  Command opcode.
 @param Arg                     Command argument.
 @param ResponseType            MMC response type.
 @param OutResponse             Pointer to variable to receive response.
                                Should be 4 32-bit words long for R2 response.
                                For other response types 1 word will be enough.

 @retval EFI_SUCCESS            Command executed.
 @retval EFI_TIMEOUT            Command timed out.
 @retval EFI_DEVICE_ERROR       Any non-timeout device error.
 @retval EFI_INVALID_PARAMETER  Mmc is NULL, Opcode or ResponseType is invalid,
                                Data is NULL with DataLength > 0,
                                DataLength is not multiple of BlockLength
**/
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
);

/**
 Sets MMC bus clock frequency and bus width.

 @param Mmc                     Pointer to Mmc driver instance
 @param Hz                      Bus frequency in Hz.
 @param BusWidth                Bus width.

 @retval EFI_SUCCESS            Bus frequency and width set.
 @retval EFI_TIMEOUT            Clock update timed out.
 @retval EFI_DEVICE_ERROR       Any device error other than timeout.
 @retval EFI_UNSUPPORTED        Frequency or bus width is not supported by host.
 @retval EFI_INVALID_PARAMETER  Mmc is NULL.
**/
EFI_STATUS SunxiMmcSetIos(
  IN SUNXI_MMC *Mmc,
  IN UINT32 Hz,
  IN UINT32 BusWidth
);

#define CARD_TYPE_SD 1
#define CARD_TYPE_EMMC 2
EFI_STATUS SunxiMmcDetectCard(
  IN SUNXI_MMC *Mmc,
  OUT BOOLEAN *IsInserted,
  OUT UINT32 *CardType OPTIONAL
);

EFI_STATUS SunxiMmcUnmaskGate(
  IN SUNXI_MMC *Mmc,
  IN CONST CHAR16 *Gate
);

EFI_STATUS SunxiMmcMaskGate(
  IN SUNXI_MMC *Mmc,
  IN CONST CHAR16 *Gate
);
