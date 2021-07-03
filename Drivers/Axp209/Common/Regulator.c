#include "Driver.h"

#define AXP209_REGULATOR(_Name, _Min, _Max, _Step, _Vreg, _Vshift, _Vwidth, _EnBit, _Flags) \
  {                                                                                         \
    .Name = (_Name),                                                                        \
    .MinVoltage = (_Min),                                                                   \
    .MaxVoltage = (_Max),                                                                   \
    .Step = (_Step),                                                                        \
    .VoltageReg = (_Vreg),                                                                  \
    .VoltageShift = (_Vshift),                                                              \
    .VoltageWidth = (_Vwidth),                                                              \
    .EnableBit = (_EnBit),                                                                  \
    .VoltageListLength = 0,                                                                 \
    .Flags = (_Flags),                                                                      \
  }

#define AXP209_REGULATOR_VL(_Name, _Min, _Max, _Vreg, _Vshift, _Vwidth, _EnBit, _Flags, _Vlist) \
  {                                                                                             \
    .Name = (_Name),                                                                            \
    .MinVoltage = (_Min),                                                                       \
    .MaxVoltage = (_Max),                                                                       \
    .Step = 0,                                                                                  \
    .VoltageReg = (_Vreg),                                                                      \
    .VoltageShift = (_Vshift),                                                                  \
    .VoltageWidth = (_Vwidth),                                                                  \
    .EnableBit = (_EnBit),                                                                      \
    .VoltageListLength = ARRAY_SIZE(_Vlist),                                                    \
    .Flags = (_Flags),                                                                          \
    .VoltageList = (_Vlist),                                                                    \
  }

STATIC CONST UINT16 mLdo4VoltageList[] = {
  1250, 1300, 1400, 1500, 1600, 1700, 1800, 1900,
  2000, 2500, 2700, 2800, 3000, 3100, 3200, 3300
};

STATIC CONST AXP209_REGULATOR mRegulatorList[] = {
  [AXP209_REGULATOR_DCDC2] = AXP209_REGULATOR(L"dcdc2", 700, 2275, 25, AXP209_REG_DCDC2_VOLTAGE, 0, 8, AXP209_OUTPUT_CTRL_DCDC2, AXP209_REGULATOR_CRITICAL | AXP209_REGULATOR_MANUAL_RAMP), // DCDC2 powers CPU
  [AXP209_REGULATOR_DCDC3] = AXP209_REGULATOR(L"dcdc3", 700, 3500, 25, AXP209_REG_DCDC3_VOLTAGE, 0, 8, AXP209_OUTPUT_CTRL_DCDC3, 0),
  [AXP209_REGULATOR_ALDO2] = AXP209_REGULATOR(L"aldo2", 1800, 3300, 100, AXP209_REG_LDO24_VOLTAGE, 4, 4, AXP209_OUTPUT_CTRL_LDO2, 0),
  [AXP209_REGULATOR_ALDO3] = AXP209_REGULATOR(L"aldo3", 700, 3500, 25, AXP209_REG_LDO3_VOLTAGE, 0, 8, AXP209_OUTPUT_CTRL_LDO3, AXP209_REGULATOR_DENY),
  [AXP209_REGULATOR_ALDO4] = AXP209_REGULATOR_VL(L"aldo4", 1250, 3300, AXP209_REG_LDO24_VOLTAGE, 0, 4, AXP209_OUTPUT_CTRL_LDO4, AXP209_REGULATOR_DENY, mLdo4VoltageList),
};

EFI_STATUS AxpGetRegulator(
  IN UINT32 Regulator,
  OUT CONST AXP209_REGULATOR **OutRegulator
  )
{
  if (OutRegulator == NULL)
    return EFI_INVALID_PARAMETER;
  
  if (Regulator >= ARRAY_SIZE(mRegulatorList))
    return EFI_NOT_FOUND;

  *OutRegulator = &mRegulatorList[Regulator];

  return EFI_SUCCESS;
}

EFI_STATUS AxpGetRegulatorState(
  IN AXP209_DRIVER *Driver,
  IN CONST AXP209_REGULATOR *Regulator,
  OUT BOOLEAN *OutIsEnabled,
  OUT UINT32 *OutVoltage OPTIONAL
  )
{
  EFI_STATUS Status;
  UINT8 Data;

  if (Driver == NULL || OutIsEnabled == NULL || Regulator == NULL)
    return EFI_INVALID_PARAMETER;

  Status = AxpRead8(Driver, AXP209_REG_OUTPUT_CTRL, &Data);
  if (EFI_ERROR(Status))
    return Status;

  *OutIsEnabled = (Data & Regulator->EnableBit) == Regulator->EnableBit;

  if (OutVoltage) {
    Status = AxpRead8(Driver, Regulator->VoltageReg, &Data);
    if (EFI_ERROR(Status))
      return Status;

    Data >>= Regulator->VoltageShift;
    Data &= ((1u << (UINT32)Regulator->VoltageWidth)) - 1;

    if (Regulator->VoltageList == NULL)
      *OutVoltage = (Data * Regulator->Step) + Regulator->MinVoltage;
    else {
      ASSERT(Data < Regulator->VoltageListLength);

      if (Data < Regulator->VoltageListLength)
        *OutVoltage = Regulator->VoltageList[Data];
      else
        return EFI_DEVICE_ERROR;
    }
  }

  return Status;
}

EFI_STATUS AxpSetRegulatorVoltage(
  IN AXP209_DRIVER *Driver,
  IN CONST AXP209_REGULATOR *Regulator,
  IN UINT32 Voltage
  )
{
  EFI_STATUS Status;
  UINT8 CtrlReg;
  UINT8 LowestVoltageCfg = 0;
  UINT8 DesiredVoltageCfg = 0;
  UINT8 CurrentVoltageCfg = 0;
  INT32 i;
  BOOLEAN Found = FALSE;
  BOOLEAN IsEnabled = FALSE;
  UINT8 SavedVoltageReg;
  UINT8 VoltageReg;
  UINT8 VoltageMask;
  BOOLEAN DisableRegulator = FALSE;

  if (Driver == NULL || Regulator == NULL)
    return EFI_INVALID_PARAMETER;

  if (Voltage != 0 && (Voltage < Regulator->MinVoltage || Voltage > Regulator->MaxVoltage))
    return EFI_UNSUPPORTED;

  if (Voltage == 0)
    DisableRegulator = TRUE;

  if (!DisableRegulator) {
    if (Regulator->Step > 0) {
      if (Voltage % Regulator->Step != 0)
        return EFI_UNSUPPORTED;
      DesiredVoltageCfg = (Voltage - Regulator->MinVoltage) / Regulator->Step;
    }
    else {
      ASSERT(Regulator->VoltageList != NULL);
      ASSERT(Regulator->VoltageListLength > 0);

      if (Regulator->VoltageList != NULL && Regulator->VoltageListLength > 0) {
        for (i = 0; i < Regulator->VoltageListLength; i++) {
          if (Regulator->VoltageList[i] == Voltage) {
            Found = TRUE;
            DesiredVoltageCfg = i;
          }
        }
        if (!Found)
          return EFI_UNSUPPORTED;
      } else
        return EFI_UNSUPPORTED;
    }
  }

  if (Regulator->Flags & AXP209_REGULATOR_DENY)
    return EFI_ACCESS_DENIED;
  
  if (DisableRegulator && Regulator->Flags & AXP209_REGULATOR_CRITICAL)
    return EFI_ACCESS_DENIED;

  Status = AxpRead8(Driver, AXP209_REG_OUTPUT_CTRL, &CtrlReg);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;
  
  if (DisableRegulator) {
    // TODO: support disabling regulators
    ASSERT(0);
    return EFI_ACCESS_DENIED;
  }

  IsEnabled = (CtrlReg & Regulator->EnableBit) == Regulator->EnableBit;
  // Critical regulators must always be on
  // currently the only critical regulator is DCDC2 which powers CPU
  if (Regulator->Flags & AXP209_REGULATOR_CRITICAL) {
    if (!IsEnabled) {
      DEBUG((EFI_D_ERROR, "Critical regulator %s not enabled\n", Regulator->Name));
      return EFI_DEVICE_ERROR;
    }
  }

  VoltageMask = ((1u << (UINT32)Regulator->VoltageWidth) - 1) << Regulator->VoltageShift;
  ASSERT(((LowestVoltageCfg << Regulator->VoltageShift) & ~VoltageMask) == 0);
  ASSERT(((DesiredVoltageCfg << Regulator->VoltageShift) & ~VoltageMask) == 0);

  if (Regulator->Flags & AXP209_REGULATOR_MANUAL_RAMP) {
    Status = AxpRead8(Driver, Regulator->VoltageReg, &VoltageReg);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status))
      return Status;

    if (!IsEnabled) {
      VoltageReg &= ~VoltageMask;
      VoltageReg |= LowestVoltageCfg << Regulator->VoltageShift;

      Status = AxpWrite8(Driver, Regulator->VoltageReg, VoltageReg);
      ASSERT_EFI_ERROR(Status);
      if (EFI_ERROR(Status))
        return Status;

      CtrlReg |= Regulator->EnableBit;
      Status = AxpWrite8(Driver, AXP209_REG_OUTPUT_CTRL, CtrlReg);
      ASSERT_EFI_ERROR(Status);
      if (EFI_ERROR(Status))
        return Status;
    }

    CurrentVoltageCfg = (VoltageReg & VoltageMask) >> Regulator->VoltageShift;
    ASSERT((CurrentVoltageCfg & ~VoltageMask) == 0);
    DEBUG((EFI_D_ERROR, "CurrentVoltageCfg = 0x%x, DesiredVoltageCfg = 0x%x\n", CurrentVoltageCfg, DesiredVoltageCfg));

    if (CurrentVoltageCfg == DesiredVoltageCfg)
      return EFI_SUCCESS;
    else if (CurrentVoltageCfg > DesiredVoltageCfg) {
      CurrentVoltageCfg -= 1;
      for (; CurrentVoltageCfg >= DesiredVoltageCfg; CurrentVoltageCfg -= 1) {
        VoltageReg &= ~VoltageMask;
        VoltageReg |= CurrentVoltageCfg << Regulator->VoltageShift;

        DEBUG((
          EFI_D_ERROR,
          "Set voltage: cfg=0x%x desired=0x%x mask=0x%x shift=0x%x WRITE reg 0x%x = 0x%x\n",
          CurrentVoltageCfg,
          DesiredVoltageCfg,
          VoltageMask,
          Regulator->VoltageShift,
          Regulator->VoltageReg,
          VoltageReg
        ));

        if (Regulator->VoltageShift == 0 && Regulator->VoltageWidth == 8) {
          Status = AxpWrite8(Driver, Regulator->VoltageReg, VoltageReg);
          ASSERT_EFI_ERROR(Status);
          if (EFI_ERROR(Status))
            return Status;
          // TODO
        } else ASSERT(0);
      }
    } else if (CurrentVoltageCfg < DesiredVoltageCfg) {
      CurrentVoltageCfg += 1;
      for (; CurrentVoltageCfg <= DesiredVoltageCfg; CurrentVoltageCfg += 1) {
        VoltageReg &= ~VoltageMask;
        VoltageReg |= CurrentVoltageCfg << Regulator->VoltageShift;

        DEBUG((
          EFI_D_ERROR,
          "Set voltage: cfg=0x%x desired=0x%x mask=0x%x shift=0x%x WRITE reg 0x%x = 0x%x\n",
          CurrentVoltageCfg,
          DesiredVoltageCfg,
          VoltageMask,
          Regulator->VoltageShift,
          Regulator->VoltageReg,
          VoltageReg
        ));

        if (Regulator->VoltageShift == 0 && Regulator->VoltageWidth == 8) {
          Status = AxpWrite8(Driver, Regulator->VoltageReg, VoltageReg);
          ASSERT_EFI_ERROR(Status);
          if (EFI_ERROR(Status))
            return Status;
          // TODO
        } else ASSERT(0);
      }
    }
  } else {
    if (Regulator->VoltageWidth == 8 && Regulator->VoltageShift == 0)
      Status = AxpWrite8(Driver, Regulator->VoltageReg, DesiredVoltageCfg);
    else {
      Status = AxpRead8(Driver, Regulator->VoltageReg, &VoltageReg);
      ASSERT_EFI_ERROR(Status);
      if (EFI_ERROR(Status))
        return Status;

      CurrentVoltageCfg = (VoltageReg & VoltageMask) >> Regulator->VoltageShift;

      SavedVoltageReg = VoltageReg;

      VoltageReg &= ~VoltageMask;
      VoltageReg |= DesiredVoltageCfg << Regulator->VoltageShift;

      DEBUG((
        EFI_D_ERROR,
        "Set %s voltage to %d: set cfg 0x%x -> 0x%x (mask 0x%x): change reg 0x%x from 0x%x to 0x%x\n",
        Regulator->Name,
        Voltage,
        CurrentVoltageCfg,
        DesiredVoltageCfg,
        VoltageMask,
        Regulator->VoltageReg,
        SavedVoltageReg,
        VoltageReg
      ));

      Status = AxpWrite8(Driver, Regulator->VoltageReg, VoltageReg);
    }

    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status))
      return Status;

    if (!IsEnabled) {
      CtrlReg |= Regulator->EnableBit;
      Status = AxpWrite8(Driver, AXP209_REG_OUTPUT_CTRL, CtrlReg);
      ASSERT_EFI_ERROR(Status);
      if (EFI_ERROR(Status))
        return Status;
    }
  }

  return Status;
}
