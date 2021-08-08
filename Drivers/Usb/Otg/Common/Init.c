#include "Driver.h"
#include "Hw.h"

STATIC UINT32 UsbReadConfigData(USB_DRIVER *Driver) {
  if (gSunxiSocConfig.Flags & SUNXI_USB_FIXED_CONFIG_DATA)
    return 0xde;
  else {
    MmioWrite8(Driver->Base + MUSB_INDEX, 0);
    return MmioRead8(Driver->Base + MUSB_CONFIGDATA);
  }
}

VOID UsbCoreInit(USB_DRIVER *Driver) {
  UINT32 Config;

  Config = UsbReadConfigData(Driver);
  DEBUG((EFI_D_INFO, "Config (0x%x): ", Config));

  DEBUG((EFI_D_INFO, Config & MUSB_CONFIGDATA_UTMIDW ? "UTMI-16" : "UTMI-8"));

  if (Config & MUSB_CONFIGDATA_DYNFIFO)
    DEBUG((EFI_D_INFO, ", dyn FIFOs"));

  if (Config & MUSB_CONFIGDATA_HBRXE)
    DEBUG((EFI_D_INFO, ", HB-ISO Rx"));

  if (Config & MUSB_CONFIGDATA_MPRXE)
    DEBUG((EFI_D_INFO, ", bulk combine"));

  if (Config & MUSB_CONFIGDATA_MPTXE)
    DEBUG((EFI_D_INFO, ", bulk split"));

  if (Config & MUSB_CONFIGDATA_HBTXE)
    DEBUG((EFI_D_INFO, ", HB-ISO Tx"));

  if (Config & MUSB_CONFIGDATA_SOFTCONE)
    DEBUG((EFI_D_INFO, ", SoftConn"));

  DEBUG((EFI_D_INFO, "\n"));

  UsbSetupFifo(Driver);
}

VOID UsbSetupFifo(USB_DRIVER *Driver) {
  UINT32 i;
  UINT16 Offset = 0;
  UINT32 MaxPacketTx;
  UINT32 MaxPacketRx;
  CONST USB_EP_FIFO_CONFIG *Config;

  for (i = 0; i < gSunxiSocConfig.NumEndpoints; i++) {
    Config = &gSunxiSocConfig.EpFifoConfig[i];

    MmioWrite8(Driver->Base + MUSB_INDEX, i);

    if (Config->Flags & USB_FIFO_TX) {
      MaxPacketTx = 1 << (UINT32)Config->TxMaxPacketSizeLog2;

      MmioWrite8(Driver->Base + MUSB_TXFIFOSZ, Config->TxMaxPacketSizeLog2 - 3);
      MmioWrite16(Driver->Base + MUSB_TXFIFOADD, Offset >> 3);

      DEBUG((EFI_D_INFO, "ep%d fifo tx 0x%x size %d", i, Offset, MaxPacketTx));

      if (!(Config->Flags & USB_FIFO_SHARED))
        Offset += MaxPacketTx;
      else
        ASSERT(Config->Flags & USB_FIFO_RX);
    }

    if (Config->Flags & USB_FIFO_RX) {
      MaxPacketRx = 1 << (UINT32)Config->RxMaxPacketSizeLog2;

      if (Config->Flags & USB_FIFO_SHARED)
        ASSERT(MaxPacketTx == MaxPacketRx);

      MmioWrite8(Driver->Base + MUSB_RXFIFOSZ, Config->RxMaxPacketSizeLog2 - 3);
      MmioWrite16(Driver->Base + MUSB_RXFIFOADD, Offset >> 3);

      DEBUG((EFI_D_INFO, " rx 0x%x size %d", Offset, MaxPacketRx));

      Offset += MaxPacketRx;
    }

    DEBUG((EFI_D_INFO, "\n"));
  }
}
