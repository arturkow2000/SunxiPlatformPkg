#pragma once

#include <Uefi.h>

EFI_STATUS SunxiUsbPhyInit();
/**
 Enable pull up on ID pin
**/
EFI_STATUS SunxiUsbPhyEnableIdPullup(UINT32 Phy);
/**
 Enable pull up on D- and D+
**/
EFI_STATUS SunxiUsbPhyEnableDmDpPullup(UINT32 Phy);
EFI_STATUS SunxiUsbPhyForceIdToLow(UINT32 Phy);
EFI_STATUS SunxiUsbPhyForceIdToHigh(UINT32 Phy);
EFI_STATUS SunxiUsbPhyForceVbusValidToLow(UINT32 Phy);
EFI_STATUS SunxiUsbPhyForceVbusValidToHigh(UINT32 Phy);
