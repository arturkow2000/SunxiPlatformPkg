#include "Driver.h"
#include "Hw.h"

VOID UsbHandleInterrupt(USB_DRIVER *Driver) {
  UINT8 IntrUsb;
  UINT16 IntrTx;
  UINT16 IntrRx;
  UINT16 Tmp;
  UINT32 Endpoint;

  if (!Driver->Enabled)
    return;

  IntrUsb = MmioRead8(Driver->Base + MUSB_INTRUSB);
  IntrTx = MmioRead16(Driver->Base + MUSB_INTRTX);
  IntrRx = MmioRead16(Driver->Base + MUSB_INTRRX);

  // Ignore this interrupt
  IntrUsb &= ~MUSB_INTR_SOF;

  if (IntrUsb || IntrTx || IntrRx)
    DEBUG((EFI_D_INFO, "INTR USB 0x%08x TX 0x%08x RX 0x%08x\n", IntrUsb, IntrTx, IntrRx));

  // On bus reset set device address to 0, this is required for USB to work
  // after reconnecting device
  if (IntrUsb & MUSB_INTR_RESET) {
    DEBUG((EFI_D_INFO, "USB RESET\n\n\n"));
    UsbReset(Driver);
  }

  // Handle incoming data on EP0
  if (IntrTx & 1)
    UsbEp0HandleIrq(Driver);

  // Handle RX on EP1 - EP15
  if (IntrRx) {
    Tmp = IntrRx >> 1;
    Endpoint = 1;

    while (Tmp) {
      if (Tmp & 1)
        UsbEpxHandleRxIrq(Driver, Endpoint);

      Tmp >>= 1;
      Endpoint++;
    }
  }

  // Handle TX on EP1 - EP15
  if (IntrTx) {
    Tmp = IntrTx >> 1;
    Endpoint = 1;

    while (Tmp) {
      if (Tmp & 1)
        UsbEpxHandleTxIrq(Driver, Endpoint);

      Tmp >>= 1;
      Endpoint++;
    }
  }

  // Clear interrupt status
  if (IntrUsb)
    MmioWrite8(Driver->Base + MUSB_INTRUSB, IntrUsb);
  
  if (IntrTx)
    MmioWrite16(Driver->Base + MUSB_INTRTX, IntrTx);

  if (IntrRx)
    MmioWrite16(Driver->Base + MUSB_INTRRX, IntrRx);
}