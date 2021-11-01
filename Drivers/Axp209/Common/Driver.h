#pragma once

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

#include "Hw.h"

typedef UINT8 AXP209_DRIVER;

enum AXP209_REGULATOR_ID {
  AXP209_REGULATOR_DCDC2 = 0,
  AXP209_REGULATOR_DCDC3,
  AXP209_REGULATOR_ALDO2,
  AXP209_REGULATOR_ALDO3,
  AXP209_REGULATOR_ALDO4
};

#define AXP209_REGULATOR_CRITICAL (1 << 0)
#define AXP209_REGULATOR_MANUAL_RAMP (1 << 1)

#define AXP209_REGULATOR_DENY (1 << 7)

typedef struct {
  CONST CHAR16 *Name;
  UINT32 MinVoltage;
  UINT32 MaxVoltage;
  UINT32 Step;
  UINT8 VoltageReg;
  UINT8 VoltageShift;
  UINT8 VoltageWidth;
  UINT8 EnableBit;
  UINT8 VoltageListLength;
  CONST UINT16 *VoltageList;
  UINT8 Flags;
} AXP209_REGULATOR;

EFI_STATUS AxpGetRegulator(
  IN UINT32 Regulator,
  OUT CONST AXP209_REGULATOR **OutRegulator
);

EFI_STATUS AxpGetRegulatorState(
  IN AXP209_DRIVER *Driver,
  IN CONST AXP209_REGULATOR *Regulator,
  OUT BOOLEAN *OutIsEnabled,
  OUT UINT32 *OutVoltage OPTIONAL
);

EFI_STATUS AxpSetRegulatorVoltage(
  IN AXP209_DRIVER *Driver,
  IN CONST AXP209_REGULATOR *Regulator,
  IN UINT32 Voltage
);

EFI_STATUS AxpRead8(
  IN AXP209_DRIVER *Driver,
  IN UINT8 Reg,
  OUT UINT8 *OutData
);

EFI_STATUS AxpWrite8(
  IN AXP209_DRIVER *Driver,
  IN UINT8 Reg,
  IN UINT8 Data
);

EFI_STATUS AxpGpioOutput(
  IN AXP209_DRIVER *Driver,
  IN UINT32 Pin,
  IN UINT32 Value
);

EFI_STATUS AxpGpioInput(
  IN AXP209_DRIVER *Driver,
  IN UINT32 Pin
);
