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

STATIC inline CONST SUNXI_GPIO_PIN_INTERNAL *SunxiGpioPinGetInternal(IN SUNXI_GPIO_PIN Pin)
{
  if (Pin >= gSunxiGpioPinListLength)
    return NULL;

  if (gSunxiGpioPinList[Pin].MuxTable == NULL)
    return NULL;

  return &gSunxiGpioPinList[Pin];
}

STATIC SUNXI_GPIO_PIN SunxiGpioPinForBank(IN UINTN Bank, IN UINTN Pin)
{
  UINTN Index;

  if (Pin >= _GPIO_PINS_PER_BANK)
    return SUNXI_GPIO_PIN_INVALID;

  Index = Bank * _GPIO_PINS_PER_BANK + Pin;

  if (Index >= gSunxiGpioPinListLength)
    return SUNXI_GPIO_PIN_INVALID;

  if (!SunxiGpioPinGetInternal((SUNXI_GPIO_PIN)Index))
    return SUNXI_GPIO_PIN_INVALID;

  return Index;
}

SUNXI_GPIO_PIN SunxiGpioNameToPin(IN CONST CHAR16 *Name)
{
  CHAR16 *Ptr;
  UINTN NameLen;
  UINTN Bank;
  UINTN Pin;
  EFI_STATUS Status;

  NameLen = StrLen(Name);

  if (NameLen != 4)
    return SUNXI_GPIO_PIN_INVALID;

  if (Name[0] != L'P')
    return SUNXI_GPIO_PIN_INVALID;

  if (!(Name[1] >= L'A' && Name[1] <= L'Z'))
    return SUNXI_GPIO_PIN_INVALID;

  Bank = Name[1] - L'A';

  Status = StrDecimalToUintnS(&Name[2], &Ptr, &Pin);
  ASSERT_EFI_ERROR(Status);

  if ((UINTN)Ptr != (UINTN)&Name[4])
    return SUNXI_GPIO_PIN_INVALID;

  return SunxiGpioPinForBank(Bank, Pin);
}

BOOLEAN SunxiGpioIsPinValid(IN SUNXI_GPIO_PIN Pin)
{
  return !!SunxiGpioPinGetInternal(Pin);
}

UINTN SunxiGpioGetPinCount(VOID)
{
  return gSunxiGpioPinListLength;
}

BOOLEAN SunxiGpioGetPinName(IN SUNXI_GPIO_PIN Pin, IN CHAR16 *Buffer, IN UINTN BufferLength, OUT UINTN *OutBufferLength OPTIONAL)
{
  UINTN Temp;

  ASSERT(SunxiGpioIsPinValid(Pin));

  if (BufferLength < 5)
  {
    if (OutBufferLength)
      *OutBufferLength = 5;

    return FALSE;
  }

  Temp = UnicodeSPrint(Buffer, BufferLength, L"P%c%02d", L'A' + Pin / _GPIO_PINS_PER_BANK, Pin % _GPIO_PINS_PER_BANK);
  if (OutBufferLength)
    *OutBufferLength = Temp;

  return TRUE;
}

CONST CHAR16 *SunxiGpioMuxGetFunction(IN SUNXI_GPIO_PIN Pin)
{
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;
  INTN i;
  UINT32 Offset;
  UINT32 Shift;
  UINT32 Reg;
  UINT32 Function;

  Internal = SunxiGpioPinGetInternal(Pin);
  if (Internal)
  {
    ASSERT(Internal->MuxTable);
    ASSERT(Internal->MuxTableSize > 0);

    Offset = (Pin / _GPIO_PINS_PER_BANK) * BANK_MEM_SIZE + (Pin % _GPIO_PINS_PER_BANK) / MUX_PINS_PER_REG * 4;
    Shift = ((Pin % _GPIO_PINS_PER_BANK) % MUX_PINS_PER_REG) * MUX_PINS_BITS;
    Reg = MmioRead32(GPIO_BASE + Offset);
    Function = (Reg >> Shift) & MUX_PINS_MASK;

    for (i = 0; i < Internal->MuxTableSize; i++)
    {
      if (Internal->MuxTable[i].Id == Function)
        return Internal->MuxTable[i].Name;
    }

    DEBUG((EFI_D_ERROR, "GPIO: Unkown function %d on %c%02d\n", Function, 'A' + Pin / _GPIO_PINS_PER_BANK, Pin % _GPIO_PINS_PER_BANK));
  }

  ASSERT(0);
  return NULL;
}

BOOLEAN SunxiGpioMuxSetFunction(IN SUNXI_GPIO_PIN Pin, IN CONST CHAR16 *FunctionName)
{
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;
  INTN i;
  UINT32 Offset;
  UINT32 Shift;
  UINT32 Function;

  Function = 0xFFFFFFFF;

  Internal = SunxiGpioPinGetInternal(Pin);
  if (Internal)
  {
    ASSERT(Internal->MuxTable);
    ASSERT(Internal->MuxTableSize > 0);

    for (i = 0; i < Internal->MuxTableSize; i++)
    {
      if (StrCmp(FunctionName, Internal->MuxTable[i].Name) == 0)
      {
        Function = Internal->MuxTable[i].Id;
        break;
      }
    }

    if (Function == 0xFFFFFFFF)
      return FALSE;

    Offset = (Pin / _GPIO_PINS_PER_BANK) * BANK_MEM_SIZE + (Pin % _GPIO_PINS_PER_BANK) / MUX_PINS_PER_REG * 4;
    Shift = ((Pin % _GPIO_PINS_PER_BANK) % MUX_PINS_PER_REG) * MUX_PINS_BITS;

    MmioAndThenOr32(GPIO_BASE + Offset, ~(MUX_PINS_MASK << Shift), Function << Shift);

    return TRUE;
  }
  else
    ASSERT(0);

  return FALSE;
}

CONST CHAR16 *SunxiGpioMuxGetFunctionName(IN SUNXI_GPIO_PIN Pin, IN INTN Index)
{
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;

  Internal = SunxiGpioPinGetInternal(Pin);
  if (Internal)
  {
    ASSERT(Internal->MuxTable);
    ASSERT(Internal->MuxTableSize);

    if (Index >= Internal->MuxTableSize)
      return NULL;

    return Internal->MuxTable[Index].Name;
  }

  ASSERT(0);
  return NULL;
}

UINT32 SunxiGpioPullGet(IN SUNXI_GPIO_PIN Pin)
{
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;
  UINT32 Offset;
  UINT32 Shift;

  Internal = SunxiGpioPinGetInternal(Pin);
  if (Internal)
  {
    ASSERT(Internal->MuxTable);
    ASSERT(Internal->MuxTableSize > 0);

    Offset = PULL_REGS_OFFSET + ((Pin / _GPIO_PINS_PER_BANK) * BANK_MEM_SIZE + (Pin % _GPIO_PINS_PER_BANK) / PULL_PINS_PER_REG * 4);
    Shift = ((Pin % _GPIO_PINS_PER_BANK) % PULL_PINS_PER_REG) * PULL_PINS_BITS;

    return (MmioRead32(GPIO_BASE + Offset) >> Shift) & PULL_PINS_MASK;
  }

  ASSERT(0);
  return 0;
}

VOID SunxiGpioPullSet(IN SUNXI_GPIO_PIN Pin, IN UINT32 Pull)
{
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;
  UINT32 Offset;
  UINT32 Shift;

  if ((Pull & ~PULL_PINS_MASK) != 0)
  {
    ASSERT(0);
    return;
  }

  Internal = SunxiGpioPinGetInternal(Pin);
  if (Internal)
  {
    ASSERT(Internal->MuxTable);
    ASSERT(Internal->MuxTableSize > 0);

    Offset = PULL_REGS_OFFSET + ((Pin / _GPIO_PINS_PER_BANK) * BANK_MEM_SIZE + (Pin % _GPIO_PINS_PER_BANK) / PULL_PINS_PER_REG * 4);
    Shift = ((Pin % _GPIO_PINS_PER_BANK) % PULL_PINS_PER_REG) * PULL_PINS_BITS;

    MmioAndThenOr32(GPIO_BASE + Offset, ~(PULL_PINS_MASK << Shift), Pull << Shift);
  }
  else
    ASSERT(0);
}

UINT32 SunxiGpioGetDriveStrength(IN SUNXI_GPIO_PIN Pin)
{
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;
  UINT32 Offset;
  UINT32 Shift;
  UINT32 Reg;

  Internal = SunxiGpioPinGetInternal(Pin);
  if (Internal)
  {
    ASSERT(Internal->MuxTable);
    ASSERT(Internal->MuxTableSize > 0);

    Offset = DLEVEL_REGS_OFFSET + ((Pin / _GPIO_PINS_PER_BANK) * BANK_MEM_SIZE + (Pin % _GPIO_PINS_PER_BANK) / DLEVEL_REGS_OFFSET * 4);
    Shift = ((Pin % _GPIO_PINS_PER_BANK) % DLEVEL_PINS_PER_REG) * DLEVEL_PINS_BITS;

    Reg = (MmioRead32(GPIO_BASE + Offset) >> Shift) & DLEVEL_PINS_MASK;
    return (Reg + 1) * 10;
  }

  ASSERT(0);
  return 0;
}

VOID SunxiGpioSetDriveStrength(IN SUNXI_GPIO_PIN Pin, IN UINT32 Strength)
{
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;
  UINT32 Offset;
  UINT32 Shift;
  UINT32 Raw;

  Internal = SunxiGpioPinGetInternal(Pin);
  if (Internal)
  {
    ASSERT(Internal->MuxTable);
    ASSERT(Internal->MuxTableSize > 0);

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
    default:
      return;
      break;
    }

    MmioAndThenOr32(GPIO_BASE + Offset, ~(DLEVEL_PINS_MASK << Shift), Raw << Shift);
  }
  else
    ASSERT(0);
}

UINT32 SunxiGpioRead(IN SUNXI_GPIO_PIN Pin)
{
  CONST SUNXI_GPIO_PIN_INTERNAL *Internal;
  UINT32 Offset;
  UINT32 Shift;

  Internal = SunxiGpioPinGetInternal(Pin);
  if (Internal)
  {
    ASSERT(Internal->MuxTable);
    ASSERT(Internal->MuxTableSize > 0);

    Offset = DATA_REGS_OFFSET + ((Pin / _GPIO_PINS_PER_BANK) * BANK_MEM_SIZE + (Pin % _GPIO_PINS_PER_BANK) / DATA_REGS_OFFSET * 4);
    Shift = ((Pin % _GPIO_PINS_PER_BANK) % DATA_PINS_PER_REG) * DATA_PINS_PER_REG;

    return (MmioRead32(GPIO_BASE + Offset) >> Shift) & DATA_PINS_MASK;
  }

  ASSERT(0);
  return 0;
}
