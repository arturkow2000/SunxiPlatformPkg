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

extern EFI_GUID gSunxiUsbPhyPpiGuid;
SUNXI_CCM_PPI *gSunxiCcmPpi = NULL;

STATIC EFI_PEI_PPI_DESCRIPTOR mPpiDesc;

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

  Status = PeiServicesAllocatePool(
    sizeof(USB_PHY_PEI_DRIVER),
    (VOID**)&Driver
  );
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Driver->Common.SocConfig = &gUsbPhySocConfig;

  Status = UsbPhyInit(&Driver->Common);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  // currently phy driver does not expose any api to outside world
  // we only intialize phy for operation in gadget mode
  // and install dummy interface to signal musb driver
  mPpiDesc.Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  mPpiDesc.Guid = &gSunxiUsbPhyPpiGuid;
  mPpiDesc.Ppi = NULL;
  Status = PeiServicesInstallPpi(&mPpiDesc);
  ASSERT_EFI_ERROR(Status);

  return Status;
}
