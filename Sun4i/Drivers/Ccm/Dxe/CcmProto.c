/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (c) Artur Kowalski. <arturkow2000@gmail.com>
**/

#include "Driver.h"
#include "../Common/Driver.h"

STATIC
EFI_STATUS
EFIAPI
GetGate(
  IN  SUNXI_CCM_PROTOCOL      *This,
  IN  CONST CHAR16            *Name,
  OUT UINT32                  *OutGateId
  )
{
  INT32 i;

  if (Name == NULL || OutGateId == NULL)
    return EFI_INVALID_PARAMETER;

  for (i = 0; i < gCcmGateListLength; i++) {
    if (StrCmp(Name, gCcmGateList[i].Name) == 0) {
      // Or with magic value to be able to distinguish different ID types.
      *OutGateId = i | (0xAF << 24);
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

STATIC
EFI_STATUS
EFIAPI
GetNextGate(
  IN  SUNXI_CCM_PROTOCOL      *This,
  IN  UINT32                  Index,
  OUT UINT32                  *OutGateId,
  OUT CONST CHAR16            **OutGateName OPTIONAL
  )
{
  if (OutGateId == NULL)
    return EFI_INVALID_PARAMETER;

  if (Index >= gCcmGateListLength)
    return EFI_NOT_FOUND;

  *OutGateId = Index | (0xAF << 24);
  if (OutGateName)
    *OutGateName = gCcmGateList[Index].Name;

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
GateUnmask(
  IN  SUNXI_CCM_PROTOCOL      *This,
  IN  UINT32                  GateId
  )
{
  UINT8 Sig;

  Sig = (GateId >> 24) & 0xff;
  if (Sig != 0xAF) {
    DEBUG((EFI_D_ERROR, "Invalid gate ID 0x%x\n", GateId));
    return EFI_INVALID_PARAMETER;
  }

  return CcmGateUnmask(GateId & 0xffff);
}

STATIC
EFI_STATUS
EFIAPI
GateMask(
  IN  SUNXI_CCM_PROTOCOL      *This,
  IN  UINT32                  GateId
  )
{
  UINT8 Sig;

  Sig = (GateId >> 24) & 0xff;
  if (Sig != 0xAF) {
    DEBUG((EFI_D_ERROR, "Invalid gate ID 0x%x\n", GateId));
    return EFI_INVALID_PARAMETER;
  }
  
  return CcmGateMask(GateId & 0xffff);
}

STATIC
EFI_STATUS
EFIAPI
GateIsMasked(
  IN  SUNXI_CCM_PROTOCOL      *This,
  IN  UINT32                  GateId,
  OUT BOOLEAN                 *OutIsMasked
  )
{
  UINT8 Sig;

  Sig = (GateId >> 24) & 0xff;
  if (Sig != 0xAF) {
    DEBUG((EFI_D_ERROR, "Invalid gate ID 0x%x\n", GateId));
    return EFI_INVALID_PARAMETER;
  }
  
  return CcmGateIsMasked(GateId & 0xffff, OutIsMasked);
}

STATIC
EFI_STATUS
EFIAPI
GetPll(
  IN  SUNXI_CCM_PROTOCOL      *This,
  IN  CONST CHAR16            *Name,
  OUT UINT32                  *OutPllId
  )
{
  INT32 i;

  if (Name == NULL || OutPllId == NULL)
    return EFI_INVALID_PARAMETER;
  
  for (i = 0; i < gCcmPllListLength; i++) {
    if (StrCmp(Name, gCcmPllList[i].Name) == 0) {
      // Or with magic value to be able to distinguish different ID types.
      *OutPllId = i | (0x42 << 24);
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

STATIC
EFI_STATUS
EFIAPI
GetNextPll(
  IN  SUNXI_CCM_PROTOCOL      *This,
  IN  UINT32                  Index,
  OUT UINT32                  *OutPllId,
  OUT CONST CHAR16            **OutPllName OPTIONAL
  )
{
  if (OutPllId == NULL)
    return EFI_INVALID_PARAMETER;

  if (Index >= gCcmPllListLength)
    return EFI_NOT_FOUND;

  *OutPllId = Index | (0x42 << 24);
  if (OutPllName)
    *OutPllName = gCcmPllList[Index].Name;
  
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
PllGetFreq(
  IN  SUNXI_CCM_PROTOCOL      *This,
  IN  UINT32                  PllId,
  OUT UINT32                  *OutPllFreq
  )
{
  UINT8 Sig;
  UINT32 Index;

  if (OutPllFreq == NULL)
    return EFI_INVALID_PARAMETER;

  Sig = (PllId >> 24) & 0xff;
  if (Sig != 0x42) {
    DEBUG((EFI_D_ERROR, "Invalid PLL ID 0x%x\n", PllId));
    return EFI_INVALID_PARAMETER;
  }

  Index = PllId & 0xffff;
  if (Index >= gCcmPllListLength)
    return EFI_NOT_FOUND;

  if (gCcmPllList[Index].GetFreq == NULL)
    return EFI_UNSUPPORTED;

  return gCcmPllList[Index].GetFreq(&gCcmPllList[Index], OutPllFreq);
}

STATIC
EFI_STATUS
EFIAPI
PllSetFreq(
  IN  SUNXI_CCM_PROTOCOL       *This,
  IN  UINT32                   PllId,
  IN  UINT32                   Freq
  )
{
  UINT8 Sig;
  UINT32 Index;

  Sig = (PllId >> 24) & 0xff;
  if (Sig != 0x42) {
    DEBUG((EFI_D_ERROR, "Invalid PLL ID 0x%x\n", PllId));
    return EFI_INVALID_PARAMETER;
  }

  Index = PllId & 0xffff;
  if (Index >= gCcmPllListLength)
    return EFI_NOT_FOUND;

  if (gCcmPllList[Index].SetFreq == NULL)
    return EFI_UNSUPPORTED;
  
  return gCcmPllList[Index].SetFreq(&gCcmPllList[Index], Freq);
}

SUNXI_CCM_PROTOCOL gSunxiCcmProtocol = {
  GetGate,
  GetNextGate,
  GateUnmask,
  GateMask,
  GateIsMasked,
  GetPll,
  GetNextPll,
  PllGetFreq,
  PllSetFreq
};
