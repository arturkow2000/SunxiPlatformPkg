/**
 SPDX-License-Identifier: GPL-2.0+

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>

 Based on code Linux code
 Copyright (C) 2010-2015 Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 Pan Nan <pannan@allwinnertech.com>
 Tom Cubie <tanglaing@allwinnertech.com>
 Victor Wei <weiziheng@allwinnertech.com>

**/

#include "Driver.h"

extern EFI_GUID gSunxiI2cPeiDriverGuid;

SUNXI_CCM_PPI *gSunxiCcmPpi = NULL;
SUNXI_GPIO_PPI *gSunxiGpioPpi = NULL;

EFI_STATUS
EFIAPI
SunxiI2cInitialize(
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status;
  I2C_PEI_DRIVER *Drivers = NULL;
  EFI_PEI_PPI_DESCRIPTOR *PpiList;
  UINT32 i;
  UINT32 Flags;

  if (gSunxiI2cNumControllers == 0)
    return EFI_SUCCESS;

  Status = PeiServicesLocatePpi(
    &gSunxiCcmPpiGuid,
    0,
    NULL,
    (VOID**)&gSunxiCcmPpi
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = PeiServicesLocatePpi(
    &gSunxiGpioPpiGuid,
    0,
    NULL,
    (VOID**)&gSunxiGpioPpi
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = PeiServicesAllocatePool(
    sizeof(EFI_PEI_PPI_DESCRIPTOR) * gSunxiI2cNumControllers,
    (VOID**)&PpiList
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    goto Exit;

  Status = PeiServicesAllocatePool(
    sizeof(I2C_PEI_DRIVER) * gSunxiI2cNumControllers,
    (VOID**)&Drivers
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    goto Exit;

  for (i = 0; i < gSunxiI2cNumControllers; i++) {
    CopyMem(&Drivers[i].MasterPpi, &gI2cMasterPpi, sizeof(EFI_PEI_I2C_MASTER_PPI));
    Drivers[i].Signature = PEI_DRIVER_SIGNATURE;
    Drivers[i].Common.Config = &gSunxiI2cConfig[i];
    CopyMem(&Drivers[i].MasterPpi.Identifier, &gSunxiI2cPeiDriverGuid, sizeof(EFI_GUID));
    Drivers[i].ControllerNo = gSunxiI2cConfig[i].No;
    Drivers[i].Initialized = FALSE;

    Flags = EFI_PEI_PPI_DESCRIPTOR_PPI;
    if (i == gSunxiI2cNumControllers - 1)
      Flags |= EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;

    PpiList[i].Flags = Flags;
    PpiList[i].Guid = &gEfiPeiI2cMasterPpiGuid;
    PpiList[i].Ppi = &Drivers[i].MasterPpi;
  }

  Status = PeiServicesInstallPpi(PpiList);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    goto Exit;

  Exit:
  if (EFI_ERROR(Status))
    return Status;

  return EFI_SUCCESS;
}
