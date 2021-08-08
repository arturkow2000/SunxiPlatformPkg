#include "Driver.h"

VOID UsbReadFifo(USB_DRIVER *Driver, UINT8 Endpoint, UINT16 Length, UINT8* OutData) {
  UINTN Fifo;
  UINT32 i;

  // TODO: validate endpoint

  Fifo = Driver->Base + Endpoint * 4;

  // TODO: optimize this
  for (i = 0; i < Length; i++) {
    OutData[i] = MmioRead8(Fifo);
  }
}
