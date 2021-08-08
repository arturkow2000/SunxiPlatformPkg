#include "Driver.h"
#include "Hw.h"

VOID UsbHandleInterrupt(USB_DRIVER *Driver) {
  UINT8 IntrUsb;
  UINT16 IntrTx;
  UINT16 IntrRx;

  if (!Driver->Enabled)
    return;

  IntrUsb = MmioRead8(Driver->Base + MUSB_INTRUSB);
  IntrTx = MmioRead16(Driver->Base + MUSB_INTRTX);
  IntrRx = MmioRead16(Driver->Base + MUSB_INTRRX);

  // Ignore this interrupt
  IntrUsb &= ~MUSB_INTR_SOF;

  if (IntrUsb || IntrTx || IntrRx)
    DEBUG((EFI_D_INFO, "INTR USB 0x%08x TX 0x%08x RX 0x%08x\n", IntrUsb, IntrTx, IntrRx));

  // Handle incoming data on EP0
  if (IntrTx & 1)
    UsbEp0HandleIrq(Driver);

  // Clear interrupt status
  if (IntrUsb)
    MmioWrite8(Driver->Base + MUSB_INTRUSB, IntrUsb);
  
  if (IntrTx)
    MmioWrite16(Driver->Base + MUSB_INTRTX, IntrTx);

  if (IntrRx)
    MmioWrite16(Driver->Base + MUSB_INTRRX, IntrRx);
}