/**
 SPDX-License-Identifier: GPL-2.0+

 Allwinner sun4i USB PHY driver

 Copyright (C) 2021 Artur Kowalski <arturkow2000@gmail.com>
 Copyright (C) 2017 Jagan Teki <jagan@amarulasolutions.com>
 Copyright (C) 2015 Hans de Goede <hdegoede@redhat.com>
 Copyright (C) 2014 Roman Byshko <rbyshko@gmail.com>

 Modelled arch/arm/mach-sunxi/usb_phy.c to compatible with generic-phy.

**/

#include "Driver.h"

SUNXI_CCM_PPI *gSunxiCcmPpi = NULL;
SUNXI_GPIO_PPI *gSunxiGpioPpi = NULL;

EFI_STATUS
EFIAPI
UsbPhyPeiInitialize(
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status;
  USB_PHY_PEI_DRIVER *Driver;
  
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

  Driver = PeiServicesAllocatePool(
    sizeof(USB_PHY_PEI_DRIVER),
    (VOID**)&Driver
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Driver->Common.SocConfig = &gUsbPhySocConfig;

  Status = UsbPhyInit(&Driver->Common);
  ASSERT_EFI_ERROR(Status);

  ASSERT(0);
  return EFI_SUCCESS;
}
