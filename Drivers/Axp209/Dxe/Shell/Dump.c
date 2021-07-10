#include "Ext.h"

STATIC
VOID
EFIAPI
DumpPowerStatus(VOID)
{
  EFI_STATUS Status;
  UINT8 Reg;

  Status = AxpRead8(&gAxp209DxeDriver->Common, AXP209_REG_POWER_STATUS, &Reg);
  if (EFI_ERROR(Status))
    ShellPrintEx(-1, -1, L"Failed to read power status: %r\r\n", Status);
  else {
    ShellPrintEx(-1, -1, L"Battery:");
    if (Reg & AXP209_POWER_STATUS_BAT_PRESENT)
      ShellPrintEx(-1, -1, L" PRESENT");
    if (Reg & AXP209_POWER_STATUS_BAT_ACTIVATED)
      ShellPrintEx(-1, -1, L" ACTIVE");
    if (Reg & AXP209_POWER_STATUS_BAT_CHARGING)
      ShellPrintEx(-1, -1, L" CHARGING");
    else
      ShellPrintEx(-1, -1, L" DISCHARGING");
    
    ShellPrintEx(-1, -1, L"\r\n");
  }
}

STATIC
VOID
EFIAPI
DumpRegulators(VOID)
{
  EFI_STATUS Status;
  UINT32 i;
  CONST AXP209_REGULATOR *Regulator;
  BOOLEAN IsEnabled;
  UINT32 Mvolt;

  for (i = 0; ; i++) {
    Status = AxpGetRegulator(i, &Regulator);
    if (Status == EFI_SUCCESS) {
      Status = AxpGetRegulatorState(&gAxp209DxeDriver->Common, Regulator, &IsEnabled, &Mvolt);
      if (EFI_ERROR(Status))
        ShellPrintEx(-1, -1, L"Failed to get %s state: %r\r\n", Regulator->Name, Status);
      else
        ShellPrintEx(-1, -1, L" %-10s : %4d mV %s\r\n", Regulator->Name, Mvolt, IsEnabled ? L"ON" : L"OFF");
    } else if (Status == EFI_NOT_FOUND)
      break;
    else {
      ASSERT(0);
      ShellPrintEx(-1, -1, L"Failed to get regulator %u: %r\r\n", i, Status);
      break;
    }
  }
}

SHELL_STATUS
EFIAPI
Axp209DumpCommandHandler (
  IN EFI_SHELL_DYNAMIC_COMMAND_PROTOCOL  *This,
  IN EFI_SYSTEM_TABLE                    *SystemTable,
  IN EFI_SHELL_PARAMETERS_PROTOCOL       *ShellParameters,
  IN EFI_SHELL_PROTOCOL                  *Shell
  )
{
  EFI_STATUS EfiStatus;
  SHELL_STATUS ShellStatus = SHELL_SUCCESS;

  EfiStatus = gBS->InstallMultipleProtocolInterfaces (
    &gImageHandle,
    &gEfiShellProtocolGuid, Shell,
    &gEfiShellParametersProtocolGuid, ShellParameters,
    NULL
  );
  ASSERT_EFI_ERROR(EfiStatus);
  if (EFI_ERROR(EfiStatus))
    return SHELL_ABORTED;

  EfiStatus = ShellInitialize();
  ASSERT_EFI_ERROR(EfiStatus);
  if (EFI_ERROR(EfiStatus))
    goto Exit;

  DumpPowerStatus();
  DumpRegulators();

Exit:
  gBS->UninstallMultipleProtocolInterfaces(
    gImageHandle,
    &gEfiShellProtocolGuid, Shell,
    &gEfiShellParametersProtocolGuid, ShellParameters,
    NULL
  );

  return ShellStatus;
}

CHAR16*
EFIAPI
Axp209DumpCommandGethelp (
  IN EFI_SHELL_DYNAMIC_COMMAND_PROTOCOL  *This,
  IN CONST CHAR8                         *Language
  )
{
  CHAR16 *X;

  X = HiiGetString(
    gAxp209DxeHiiHandle,
    STRING_TOKEN(STR_GET_HELP_AXP_DUMP),
    Language
  );
  ASSERT(X);

  return X;
}
