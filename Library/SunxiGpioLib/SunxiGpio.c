/*
 * SPDX-License-Identifier: GPL-2.0+
 *
 * Copyright (C) 2021, Artur Kowalski. <arturkow2000@gmail.com>
 * 
 * Based on Linux driver
 * 
 * Copyright (C) 2014-2016 Maxime Ripard <maxime.ripard@free-electrons.com>
 * Copyright (C) 2016 Mylene Josserand <mylene.josserand@free-electrons.com>
 * 
 */
#include "SunxiGpioLib.h"

STATIC inline CONST SUNXI_GPIO_PIN_INTERNAL *SunxiGpioPinGetInternal(IN UINT32 Pin)
{
  if (Pin >= gSunxiGpioPinListLength)
    return NULL;

  if (gSunxiGpioPinList[Pin].MuxTable == NULL)
    return NULL;

  ASSERT(gSunxiGpioPinList[Pin].MuxTable);
  ASSERT(gSunxiGpioPinList[Pin].MuxTableSize > 0);

  return &gSunxiGpioPinList[Pin];
}

STATIC EFI_STATUS SunxiGpioPinForBank(IN UINTN Bank, IN UINTN Pin, OUT UINT32 *OutPin)
{
  UINTN Index;

  if (Pin >= _GPIO_PINS_PER_BANK)
    return EFI_NOT_FOUND;

  Index = Bank * _GPIO_PINS_PER_BANK + Pin;

  if (Index >= gSunxiGpioPinListLength)
    return EFI_NOT_FOUND;

  if (!SunxiGpioPinGetInternal(Index))
    return EFI_NOT_FOUND;

  *OutPin = Index;

  return EFI_SUCCESS;
}

EFI_STATUS SunxiGpioGetPin(IN CONST CHAR16 *Name, OUT UINT32 *OutPin)
{
  CHAR16 *Ptr;
  UINTN NameLen;
  UINTN Bank;
  UINTN Pin;
  EFI_STATUS Status;

  if (!Name || !OutPin)
    return EFI_INVALID_PARAMETER;

  NameLen = StrLen(Name);

  if (NameLen != 4)
    return EFI_NOT_FOUND;

  if (Name[0] != L'P')
    return EFI_NOT_FOUND;

  if (!(Name[1] >= L'A' && Name[1] <= L'Z'))
    return EFI_NOT_FOUND;

  Bank = Name[1] - L'A';

  Status = StrDecimalToUintnS(&Name[2], &Ptr, &Pin);
  ASSERT_EFI_ERROR(Status);

  if ((UINTN)Ptr != (UINTN)&Name[4])
    return EFI_NOT_FOUND;

  return SunxiGpioPinForBank(Bank, Pin, OutPin);
}

EFI_STATUS SunxiGpioGetFunction(IN UINT32 Pin, OUT CONST CHAR16 **OutFunction)
{
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;
  INTN i;
  UINT32 Offset;
  UINT32 Shift;
  UINT32 Reg;
  UINT32 Function;

  if (!OutFunction)
    return EFI_INVALID_PARAMETER;

  Internal = SunxiGpioPinGetInternal(Pin);
  if (!Internal)
    return EFI_NOT_FOUND;

  Offset = (Pin / _GPIO_PINS_PER_BANK) * BANK_MEM_SIZE + (Pin % _GPIO_PINS_PER_BANK) / MUX_PINS_PER_REG * 4;
  Shift = ((Pin % _GPIO_PINS_PER_BANK) % MUX_PINS_PER_REG) * MUX_PINS_BITS;
  Reg = MmioRead32(GPIO_BASE + Offset);
  Function = (Reg >> Shift) & MUX_PINS_MASK;

  for (i = 0; i < Internal->MuxTableSize; i++)
  {
    if (Internal->MuxTable[i].Id == Function) {
      *OutFunction = Internal->MuxTable[i].Name;
      return EFI_SUCCESS;
    }
  }

  DEBUG((EFI_D_ERROR, "GPIO: Unknown function %d on %c%02d\n", Function, 'A' + Pin / _GPIO_PINS_PER_BANK, Pin % _GPIO_PINS_PER_BANK));
  ASSERT(0);

  return EFI_DEVICE_ERROR;
}

EFI_STATUS SunxiGpioSetFunction(IN UINT32 Pin, IN CONST CHAR16 *Function)
{
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;
  INTN i;
  UINT32 Offset;
  UINT32 Shift;
  UINT32 FunctionId;

  if (!Function)
    return EFI_INVALID_PARAMETER;

  FunctionId = 0xFFFFFFFF;

  Internal = SunxiGpioPinGetInternal(Pin);
  if (!Internal)
    return EFI_NOT_FOUND;

  for (i = 0; i < Internal->MuxTableSize; i++)
  {
    if (StrCmp(Function, Internal->MuxTable[i].Name) == 0)
    {
      FunctionId = Internal->MuxTable[i].Id;
      break;
    }
  }

  if (FunctionId == 0xFFFFFFFF)
    return EFI_UNSUPPORTED;

  Offset = (Pin / _GPIO_PINS_PER_BANK) * BANK_MEM_SIZE + (Pin % _GPIO_PINS_PER_BANK) / MUX_PINS_PER_REG * 4;
  Shift = ((Pin % _GPIO_PINS_PER_BANK) % MUX_PINS_PER_REG) * MUX_PINS_BITS;

  MmioAndThenOr32(GPIO_BASE + Offset, ~(MUX_PINS_MASK << Shift), FunctionId << Shift);

  return EFI_SUCCESS;
}

EFI_STATUS SunxiGpioGetPullMode(IN UINT32 Pin, OUT UINT32 *OutPullMode)
{
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;
  UINT32 Offset;
  UINT32 Shift;

  if (!OutPullMode)
    return EFI_INVALID_PARAMETER;

  Internal = SunxiGpioPinGetInternal(Pin);
  if (!Internal)
    return EFI_NOT_FOUND;

  Offset = PULL_REGS_OFFSET + ((Pin / _GPIO_PINS_PER_BANK) * BANK_MEM_SIZE + (Pin % _GPIO_PINS_PER_BANK) / PULL_PINS_PER_REG * 4);
  Shift = ((Pin % _GPIO_PINS_PER_BANK) % PULL_PINS_PER_REG) * PULL_PINS_BITS;

  *OutPullMode = (MmioRead32(GPIO_BASE + Offset) >> Shift) & PULL_PINS_MASK;
  return EFI_SUCCESS;
}

EFI_STATUS SunxiGpioSetPullMode(IN UINT32 Pin, IN UINT32 PullMode)
{
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;
  UINT32 Offset;
  UINT32 Shift;

  if ((PullMode & ~PULL_PINS_MASK) != 0)
    return EFI_UNSUPPORTED;

  Internal = SunxiGpioPinGetInternal(Pin);
  if (!Internal)
    return EFI_NOT_FOUND;

  Offset = PULL_REGS_OFFSET + ((Pin / _GPIO_PINS_PER_BANK) * BANK_MEM_SIZE + (Pin % _GPIO_PINS_PER_BANK) / PULL_PINS_PER_REG * 4);
  Shift = ((Pin % _GPIO_PINS_PER_BANK) % PULL_PINS_PER_REG) * PULL_PINS_BITS;

  MmioAndThenOr32(GPIO_BASE + Offset, ~(PULL_PINS_MASK << Shift), PullMode << Shift);

  return EFI_SUCCESS;
}

EFI_STATUS SunxiGpioGetDriveStrength(IN UINT32 Pin, OUT UINT32 *OutStrength)
{
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;
  UINT32 Offset;
  UINT32 Shift;
  UINT32 Reg;

  if (!OutStrength)
    return EFI_INVALID_PARAMETER;

  Internal = SunxiGpioPinGetInternal(Pin);
  if (!Internal)
    return EFI_NOT_FOUND;

  Offset = DLEVEL_REGS_OFFSET + ((Pin / _GPIO_PINS_PER_BANK) * BANK_MEM_SIZE + (Pin % _GPIO_PINS_PER_BANK) / DLEVEL_REGS_OFFSET * 4);
  Shift = ((Pin % _GPIO_PINS_PER_BANK) % DLEVEL_PINS_PER_REG) * DLEVEL_PINS_BITS;

  Reg = (MmioRead32(GPIO_BASE + Offset) >> Shift) & DLEVEL_PINS_MASK;
  *OutStrength = (Reg + 1) * 10;

  return EFI_SUCCESS;
}

EFI_STATUS SunxiGpioSetDriveStrength(IN UINT32 Pin, IN UINT32 Strength)
{
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;
  UINT32 Offset;
  UINT32 Shift;
  UINT32 Raw;

  Internal = SunxiGpioPinGetInternal(Pin);
  if (!Internal)
    return EFI_NOT_FOUND;

  Offset = DLEVEL_REGS_OFFSET + ((Pin / _GPIO_PINS_PER_BANK) * BANK_MEM_SIZE + (Pin % _GPIO_PINS_PER_BANK) / DLEVEL_REGS_OFFSET * 4);
  Shift = ((Pin % _GPIO_PINS_PER_BANK) % DLEVEL_PINS_PER_REG) * DLEVEL_PINS_BITS;

  switch (Strength)
  {
  case 10:
    Raw = 0;
    break;
  case 20:
    Raw = 1;
    break;
  case 30:
    Raw = 2;
    break;
  case 40:
    Raw = 3;
    break;
  default: return EFI_UNSUPPORTED;
  }

  MmioAndThenOr32(GPIO_BASE + Offset, ~(DLEVEL_PINS_MASK << Shift), Raw << Shift);
  return EFI_SUCCESS;
}

EFI_STATUS SunxiGpioConfigureAsOutput(IN UINT32 Pin) {
  return SunxiGpioSetFunction(Pin, L"gpio_out");
}

EFI_STATUS SunxiGpioConfigureAsInput(IN UINT32 Pin) {
  return SunxiGpioSetFunction(Pin, L"gpio_in");
}

EFI_STATUS SunxiGpioSetLevel(IN UINT32 Pin, IN UINT8 Level) {
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;
  UINT32 Offset;
  UINT32 Shift;

  Internal = SunxiGpioPinGetInternal(Pin);
  if (!Internal)
    return EFI_NOT_FOUND;

  Offset = DATA_REGS_OFFSET + ((Pin / _GPIO_PINS_PER_BANK) * BANK_MEM_SIZE + (Pin % _GPIO_PINS_PER_BANK) / DATA_REGS_OFFSET * 4);
  Shift = ((Pin % _GPIO_PINS_PER_BANK) % DATA_PINS_PER_REG) * DATA_PINS_PER_REG;

  MmioAndThenOr32(GPIO_BASE + Offset, ~(DATA_PINS_MASK << Shift), (!!Level) << Shift);

  return EFI_SUCCESS;
}

EFI_STATUS SunxiGpioGetLevel(IN UINT32 Pin, OUT UINT8 *OutLevel) {
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;
  UINT32 Offset;
  UINT32 Shift;

  if (!OutLevel)
    return EFI_INVALID_PARAMETER;

  Internal = SunxiGpioPinGetInternal(Pin);
  if (!Internal)
    return EFI_NOT_FOUND;

  Offset = DATA_REGS_OFFSET + ((Pin / _GPIO_PINS_PER_BANK) * BANK_MEM_SIZE + (Pin % _GPIO_PINS_PER_BANK) / DATA_REGS_OFFSET * 4);
  Shift = ((Pin % _GPIO_PINS_PER_BANK) % DATA_PINS_PER_REG) * DATA_PINS_PER_REG;

  *OutLevel = (MmioRead32(GPIO_BASE + Offset) >> Shift) & DATA_PINS_MASK;

  return EFI_SUCCESS;
}
