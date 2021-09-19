#include "Driver.h"
#include "Hw.h"

#include <Library/MemoryAllocationLib.h>

EFI_STATUS UsbInit(USB_DRIVER *Driver) {
  InitializeListHead(&Driver->Ep0Queue);

  Driver->Epx = AllocatePool(sizeof(USB_EPX) * (gSunxiSocConfig.NumEndpoints - 1));
  if (!Driver->Epx)
    return EFI_OUT_OF_RESOURCES;

  // TODO: ungate clocks before doing anything

  // disable interrupts, we are not going to use them
  MmioWrite8(Driver->Base + MUSB_INTRUSBE, 0);
  MmioWrite16(Driver->Base + MUSB_INTRTXE, 0);
  MmioWrite16(Driver->Base + MUSB_INTRRXE, 0);

  UsbReset(Driver, TRUE);

  USBC_ConfigFIFO_Base();
  USBC_EnableDpDmPullUp(Driver);
  USBC_EnableIdPullUp(Driver);
  // Currently we do not support host mode
  // set peripheral mode
  USBC_ForceIdToHigh(Driver);
  USBC_ForceVbusValidToHigh(Driver);

  UsbDisable(Driver);
  UsbCoreInit(Driver);

  return EFI_SUCCESS;
}

VOID UsbEnable(USB_DRIVER *Driver) {
  UINT8 DevCtl;

  MmioWrite8(Driver->Base + MUSB_TESTMODE, 0);

  /* put into basic highspeed mode and start session */
  MmioWrite8(Driver->Base + MUSB_POWER, MUSB_POWER_ISOUPDATE | MUSB_POWER_HSENAB | MUSB_POWER_SOFTCONN);

  DevCtl = MmioRead8(Driver->Base + MUSB_DEVCTL);
  DEBUG((EFI_D_INFO, "MUSB_DEVCTL : 0x%x VBUS : %u\n", DevCtl, (DevCtl & MUSB_DEVCTL_VBUS) >> MUSB_DEVCTL_VBUS_SHIFT));

  /* select PIO mode */
  MmioWrite8(Driver->Base + USBC_REG_o_VEND0, 0);

  USBC_ForceVbusValidToHigh(Driver);

  MmioWrite8(Driver->Base + MUSB_DEVCTL, DevCtl & ~MUSB_DEVCTL_SESSION);

  UsbReset(Driver, FALSE);

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

  // flush pending interrupts
  MmioRead8(Driver->Base + MUSB_INTRUSB);
  MmioRead16(Driver->Base + MUSB_INTRTX);
  MmioRead16(Driver->Base + MUSB_INTRRX);
}

VOID UsbReset(USB_DRIVER *Driver, BOOLEAN FirstReset) {
  UINT32 i;

  if (!FirstReset) {
    // TODO: abort all transfers
  }

  for (i = 0; i < gSunxiSocConfig.NumEndpoints - 1; i++) {
    InitializeListHead(&Driver->Epx[i].TxQueue);
    InitializeListHead(&Driver->Epx[i].RxQueue);
    Driver->Epx[i].Busy = FALSE;
    Driver->Epx[i].TxPacketSize = 0;
    Driver->Epx[i].RxPacketSize = 0;
  }

  UsbSelectEndpoint(Driver, 0);
  MmioWrite8(Driver->Base + MUSB_FADDR, 0);

  Driver->Ep0State = MUSB_EP0_STAGE_IDLE;
  Driver->AckPending = 0;
  Driver->SetAddress = FALSE;
  Driver->Address = 0;

  // Disable all endpoints, gadget driver has to re-enable these after reset
  Driver->EndpointsUsedForTx = 0;
  Driver->EndpointsUsedForRx = 0;

  // TODO: abort all pending requests
}
