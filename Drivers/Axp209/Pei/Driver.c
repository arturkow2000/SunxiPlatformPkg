#include "Driver.h"
#include <Library/PcdLib.h>

extern EFI_GUID gSunxiI2cPeiDriverGuid;
extern EFI_GUID gI2cAxp209Guid;

STATIC
EFI_STATUS
EFIAPI
AxpSetVoltage(
  IN AXP209_DRIVER *Driver,
  IN enum AXP209_REGULATOR_ID RegulatorId,
  IN UINT32 Voltage
  )
{
  EFI_STATUS Status;
  CONST AXP209_REGULATOR *Regulator;

  Status = AxpGetRegulator(RegulatorId, &Regulator);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = AxpSetRegulatorVoltage(Driver, Regulator, Voltage);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  return Status;
}

STATIC
EFI_STATUS
EFIAPI
AxpSetVoltages(
  IN AXP209_DRIVER *Driver
  )
{
  EFI_STATUS Status;

#if FixedPcdGet32(Dcdc2Voltage) == 0xffffffff
#error DCDC2 voltage is not set
#endif

#if FixedPcdGet32(Dcdc3Voltage) == 0xffffffff
#error DCDC3 voltage is not set
#endif

#if FixedPcdGet32(Ldo2Voltage) == 0xffffffff
#error LDO2 voltage is not set
#endif

  Status = AxpSetVoltage(Driver, AXP209_REGULATOR_DCDC2, FixedPcdGet32(Dcdc2Voltage));
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = AxpSetVoltage(Driver, AXP209_REGULATOR_DCDC3, FixedPcdGet32(Dcdc3Voltage));
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = AxpSetVoltage(Driver, AXP209_REGULATOR_ALDO2, FixedPcdGet32(Ldo2Voltage));
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  //ASSERT(0);

  return Status;
}

EFI_STATUS
EFIAPI
AxpInitHw(
  IN AXP209_PEI_DRIVER *Driver
  )
{
  EFI_STATUS Status;
  UINT8 Version;
  INT32 i;

  Status = AxpRead8(&Driver->Common, AXP209_REG_CHIP_VERSION, &Version);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  if ((Version & AXP209_CHIP_VERSION_MASK) != 1) {
    DEBUG((EFI_D_ERROR, "Unsupported AXP209 version: 0x%x\n", Version));
    ASSERT(0);
    return EFI_UNSUPPORTED;
  }

  /* Mask all interrupts */
  for (i = AXP209_REG_IRQ_ENABLE1; i <= AXP209_REG_IRQ_ENABLE5; i++) {
    // TODO
  }

  /*
   * Turn off LDOIO regulators / tri-state GPIO pins, when rebooting
   * from android these are sometimes on.
   */
  Status = AxpWrite8(&Driver->Common, AXP209_GPIO0_CTL, AXP209_GPIO_CTL_INPUT);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = AxpWrite8(&Driver->Common, AXP209_GPIO1_CTL, AXP209_GPIO_CTL_INPUT);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;
    
  Status = AxpWrite8(&Driver->Common, AXP209_GPIO2_CTL, AXP209_GPIO_CTL_INPUT);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = AxpSetVoltages(&Driver->Common);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FindAxp(
  IN UINT32 Controller,
  IN I2C_ENUMERATE_PPI *I2cEnumerate,
  IN EFI_GUID *Guid,
  OUT CONST EFI_I2C_DEVICE **OutDevice
  )
{
  EFI_STATUS Status;
  CONST EFI_I2C_DEVICE *Device = NULL;

  if (OutDevice == NULL) {
    ASSERT(0);
    return EFI_INVALID_PARAMETER;
  }

  while (TRUE) {
    Status = I2cEnumerate->Enumerate(I2cEnumerate, Controller, &Device);
    if (EFI_ERROR(Status))
      break;
    
    if (CompareGuid(Device->DeviceGuid, Guid)) {
      *OutDevice = Device;
      return EFI_SUCCESS;
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
Axp209Initialize(
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status;
  I2C_ENUMERATE_PPI *I2cEnumerate;
  EFI_PEI_I2C_MASTER_PPI *I2cMaster;
  INT32 i;
  UINT8 ControllerNo;
  CONST EFI_I2C_DEVICE *Device;
  AXP209_PEI_DRIVER Driver;

  Status = PeiServicesLocatePpi(
    &gI2cEnumeratePpiGuid,
    0,
    NULL,
    (VOID**)&I2cEnumerate
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  for (i = 0;; i++) {
    Status = PeiServicesLocatePpi(
      &gEfiPeiI2cMasterPpiGuid,
      i,
      NULL,
      (VOID**)&I2cMaster
    );
    if (Status == EFI_SUCCESS) {
      // Sunxi i2c driver stores controller number directly after EFI_PEI_I2C_MASTER_PPI
      if (CompareGuid(&I2cMaster->Identifier, &gSunxiI2cPeiDriverGuid)) {
        ControllerNo = ((UINT8*)(I2cMaster + 1))[0];
        Status = FindAxp(ControllerNo, I2cEnumerate, &gI2cAxp209Guid, &Device);
        if (Status == EFI_SUCCESS) {
          if (Device->SlaveAddressCount != 1) {
            DEBUG((EFI_D_ERROR, "Expected exactly 1 slave address for AXP209\n"));
            ASSERT(0);
          } else {
            DEBUG((EFI_D_INFO, "Found AXP209 at i2c%d slave 0x%x\n", ControllerNo, Device->SlaveAddressArray[0]));
            Driver.Device = Device;
            Driver.Common = 0;
            Driver.Master = I2cMaster;

            Status = AxpInitHw(&Driver);
            ASSERT_EFI_ERROR(Status);
            return Status;
          }
        } else {
          if (Status != EFI_NOT_FOUND)
            ASSERT_EFI_ERROR(Status);
        }
      }
    } else {
      if (Status != EFI_NOT_FOUND || i == 0)
        ASSERT_EFI_ERROR(Status);
      break;
    }
  }

  DEBUG((EFI_D_ERROR, "AXP209 not found\n"));
  ASSERT(0);
  return EFI_NOT_FOUND;
}
