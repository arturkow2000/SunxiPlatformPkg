#pragma once

#include <Uefi.h>

EFI_STATUS SunxiCcmGetGate(CONST CHAR16 *Name, UINT32 *OutGateId);
EFI_STATUS SunxiCcmGateUnmask(UINT32 GateId);
EFI_STATUS SunxiCcmGateMask(UINT32 GateId);
EFI_STATUS SunxiCcmGateIsMasked(UINT32 GateId, BOOLEAN *OutIsMasked);
EFI_STATUS SunxiCcmGetPll(CONST CHAR16 *Name, UINT32 *OutPllId);
EFI_STATUS SunxiCcmPllGetFreq(UINT32 PllId, UINT32 *OutPllFreq);
EFI_STATUS SunxiCcmPllSetFreq(UINT32 PllId, UINT32 PllFreq);
