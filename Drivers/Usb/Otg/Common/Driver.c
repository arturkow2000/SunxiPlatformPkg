#include "Driver.h"
#include "Hw.h"

EFI_STATUS UsbInit(USB_DRIVER *Driver) {
  InitializeListHead(&Driver->Ep0Queue);

  // TODO: ungate clocks before doing anything

  USBC_ConfigFIFO_Base();
  USBC_EnableDpDmPullUp(Driver);
  USBC_EnableIdPullUp(Driver);
  // Currently we do not support host mode
  // set peripheral mode
  USBC_ForceIdToHigh(Driver);
  USBC_ForceVbusValidToHigh(Driver);

  UsbDisable(Driver);
  UsbCoreInit(Driver);
  /*UsbEnable(Driver);

  // FIXME: remove this, in DXE when timer is available interrupt handler will be called periodically
  // in PEI phase when no timer interrupts are available
  // this will be called when printing data through emulated uart
  DEBUG((EFI_D_INFO, "Waiting for interrupts\n"));
  while (TRUE) {
    UsbHandleInterrupt(Driver);
  }*/

  return EFI_SUCCESS;
}

VOID UsbEnable(USB_DRIVER *Driver) {
  UINT8 DevCtl;

  MmioWrite8(Driver->Base + MUSB_TESTMODE, 0);

  /* put into basic highspeed mode and start session */
  //MmioWrite8(Driver->Base + MUSB_POWER, MUSB_POWER_ISOUPDATE | MUSB_POWER_HSENAB | MUSB_POWER_SOFTCONN);

  // FIXME: High speed does now work properly
  MmioWrite8(Driver->Base + MUSB_POWER, MUSB_POWER_ISOUPDATE | MUSB_POWER_SOFTCONN);

  DevCtl = MmioRead8(Driver->Base + MUSB_DEVCTL);
  DEBUG((EFI_D_INFO, "MUSB_DEVCTL : 0x%x VBUS : %u\n", DevCtl, (DevCtl & MUSB_DEVCTL_VBUS) >> MUSB_DEVCTL_VBUS_SHIFT));

  UsbSelectEndpoint(Driver, 0);
  MmioWrite8(Driver->Base + MUSB_FADDR, 0);

  /* select PIO mode */
  MmioWrite8(Driver->Base + USBC_REG_o_VEND0, 0);

  USBC_ForceVbusValidToHigh(Driver);

  MmioWrite8(Driver->Base + MUSB_DEVCTL, DevCtl & ~MUSB_DEVCTL_SESSION);

  Driver->Ep0State = MUSB_EP0_STAGE_SETUP;
  Driver->Enabled = TRUE;
}

VOID UsbDisable(USB_DRIVER *Driver) {
  Driver->Enabled = FALSE;

  USBC_ForceVbusValidToLow(Driver);
  // Wait for the current session to timeout
  MicroSecondDelay(200000);

  // terminate session
  MmioWrite8(Driver->Base + MUSB_DEVCTL, 0);
  MmioWrite8(Driver->Base + MUSB_POWER, 0);

  // disable interrupts, we are not going to use them
  MmioWrite8(Driver->Base + MUSB_INTRUSBE, 0);
  MmioWrite16(Driver->Base + MUSB_INTRTXE, 0);
  MmioWrite16(Driver->Base + MUSB_INTRRXE, 0);

  // flush pending interrupts
  MmioRead8(Driver->Base + MUSB_INTRUSB);
  MmioRead16(Driver->Base + MUSB_INTRTX);
  MmioRead16(Driver->Base + MUSB_INTRRX);
}
