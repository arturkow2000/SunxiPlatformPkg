#include "Driver.h"
#include "Hw.h"

#include <Library/MemoryAllocationLib.h>

// FIXME: SUNXI_SRAMC_BASE valid only for sun4i
#define SUNXI_SRAMC_BASE          0x01c00000

EFI_STATUS UsbInit(USB_DRIVER *Driver) {
  EFI_STATUS Status;
  UINT32 GateId;
  UINT32 i;

  InitializeListHead(&Driver->Ep0Queue);

  Driver->Base = 0x01c13000;
  Driver->Phy = 0;

  Driver->Epx = AllocatePool(sizeof(USB_EPX) * (gSunxiSocConfig.NumEndpoints - 1));
  if (!Driver->Epx)
    return EFI_OUT_OF_RESOURCES;

  for (i = 0; i < gSunxiSocConfig.NumEndpoints - 1; i++) {
    InitializeListHead(&Driver->Epx[i].TxQueue);
    InitializeListHead(&Driver->Epx[i].RxQueue);
  }

  // TODO: ungate clocks before doing anything
  Status = SunxiCcmGetGate(L"ahb0-otg", &GateId);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = SunxiCcmGateUnmask(GateId);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  // disable interrupts, we are not going to use them
  MmioWrite8(Driver->Base + MUSB_INTRUSBE, 0);
  MmioWrite16(Driver->Base + MUSB_INTRTXE, 0);
  MmioWrite16(Driver->Base + MUSB_INTRRXE, 0);

  // flush pending interrupts
  MmioRead8(Driver->Base + MUSB_INTRUSB);
  MmioRead16(Driver->Base + MUSB_INTRTX);
  MmioRead16(Driver->Base + MUSB_INTRRX);

  UsbReset(Driver, TRUE);

  // Remap SRAM bank D to USB controller (need this for FIFO to work)
  MmioAndThenOr32(
   SUNXI_SRAMC_BASE + 0x04,
   ~(0x03 << 0),
    1
  );

#if FixedPcdGet32(UsbPhySupport)
  Status = SunxiUsbPhyEnableDmDpPullup(Driver->Phy);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = SunxiUsbPhyEnableIdPullup(Driver->Phy);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  // Currently we do not support host mode
  // set peripheral mode
  Status = SunxiUsbPhyForceIdToHigh(Driver->Phy);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;

  Status = SunxiUsbPhyForceVbusValidToHigh(Driver->Phy);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status))
    return Status;
#endif

  UsbDisable(Driver);
  UsbCoreInit(Driver);

  return EFI_SUCCESS;
}

VOID UsbEnable(USB_DRIVER *Driver) {
  UINT8 DevCtl;

  MmioWrite8(Driver->Base + MUSB_TESTMODE, 0);

#if FixedPcdGet32(Usb20Support)
  MmioWrite8(Driver->Base + MUSB_POWER, MUSB_POWER_ISOUPDATE | MUSB_POWER_HSENAB | MUSB_POWER_SOFTCONN);
#else
  MmioWrite8(Driver->Base + MUSB_POWER, MUSB_POWER_ISOUPDATE | MUSB_POWER_SOFTCONN);
#endif

  DevCtl = MmioRead8(Driver->Base + MUSB_DEVCTL);
  DEBUG((EFI_D_INFO, "MUSB_DEVCTL : 0x%x VBUS : %u\n", DevCtl, (DevCtl & MUSB_DEVCTL_VBUS) >> MUSB_DEVCTL_VBUS_SHIFT));

  /* select PIO mode */
  MmioWrite8(Driver->Base + USBC_REG_o_VEND0, 0);

#if FixedPcdGet32(UsbPhySupport)
  SunxiUsbPhyForceVbusValidToHigh(Driver->Phy);
#endif

  UsbReset(Driver, FALSE);

  Driver->Enabled = TRUE;
}

VOID UsbDisable(USB_DRIVER *Driver) {
  Driver->Enabled = FALSE;

#if FixedPcdGet32(UsbPhySupport)
  SunxiUsbPhyForceVbusValidToLow(Driver->Phy);
#endif
  MmioWrite8(Driver->Base + MUSB_DEVCTL, 0);
  MmioWrite8(Driver->Base + MUSB_POWER, 0);

  // Wait for the current session to timeout
  MicroSecondDelay(200000);

  // flush pending interrupts
  MmioRead8(Driver->Base + MUSB_INTRUSB);
  MmioRead16(Driver->Base + MUSB_INTRTX);
  MmioRead16(Driver->Base + MUSB_INTRRX);
}

VOID UsbReset(USB_DRIVER *Driver, BOOLEAN FirstReset) {
  UINT32 i;
  LIST_ENTRY *Node;
  USB_REQUEST_BLOCK *Urb;

  if (!FirstReset) {
    // TODO: abort all transfers
  }

  for (Node = GetFirstNode(&Driver->Ep0Queue);
      !IsNull(&Driver->Ep0Queue, Node);
      Node = GetNextNode(&Driver->Ep0Queue, Node)) {
    Urb = USB_URB_FROM_LINK(Node);
    DEBUG((EFI_D_INFO, "Abort URB %p len %d ep0\n", Urb->Buffer, Urb->Length));
    UsbEp0CompleteRequest(Driver, Urb, EFI_ABORTED);
  }

  // TODO: abort all pending requests (on other endpoints too)
  for (i = 0; i < gSunxiSocConfig.NumEndpoints - 1; i++) {
    for (Node = GetFirstNode(&Driver->Epx[i].TxQueue);
        !IsNull(&Driver->Epx[i].TxQueue, Node);
        Node = GetNextNode(&Driver->Epx[i].TxQueue, Node)) {
      Urb = USB_URB_FROM_LINK(Node);
      DEBUG((EFI_D_INFO, "Abort URB (TX) %p len %d ep%d\n", Urb->Buffer, Urb->Length, i));
      UsbEpxCompleteRequest(Driver, Urb, EFI_ABORTED, i);
    }

    for (Node = GetFirstNode(&Driver->Epx[i].RxQueue);
        !IsNull(&Driver->Epx[i].RxQueue, Node);
        Node = GetNextNode(&Driver->Epx[i].RxQueue, Node)) {
      Urb = USB_URB_FROM_LINK(Node);
      DEBUG((EFI_D_INFO, "Abort URB (RX) %p len %d ep%d\n", Urb->Buffer, Urb->Length, i));
      UsbEpxCompleteRequest(Driver, Urb, EFI_ABORTED, i);
    }

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
}
