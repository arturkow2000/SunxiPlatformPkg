#include "Driver.h"

VOID UsbReadFifo(USB_DRIVER *Driver, UINT8 Endpoint, UINT16 Length, UINT8* OutData) {
  UINTN Fifo;
  UINT32 i = 0;
  UINT16 Left;

  // TODO: validate endpoint

  Fifo = Driver->Base + Endpoint * 4;

  // Don't care about alignment, some components of EDK II don't handle
  // alignment well on ARM so we have to keep Alignment Check off anyway.
  Left = Length;
  while (Left > 0) {
    while (Left >= 4) {
      *(UINT32*)&OutData[i] = MmioRead32(Fifo);
      i += 4;
      Left -= 4;
    }

    while (Left >= 2) {
      *(UINT16*)&OutData[i] = MmioRead16(Fifo);
      i += 2;
      Left -= 2;
    }

    if (Left > 0) {
      OutData[i] = MmioRead8(Fifo);
      Left -= 1;
    }
  }
}

VOID UsbWriteFifo(USB_DRIVER *Driver, UINT8 Endpoint, UINT16 Length, CONST UINT8* Data) {
  UINTN Fifo;
  UINT32 i = 0;
  UINT16 Left;

  // TODO: validate endpoint

  Fifo = Driver->Base + Endpoint * 4;

  // Don't care about alignment, some components of EDK II don't handle
  // alignment well on ARM so we have to keep Alignment Check off anyway.
  Left = Length;
  while (Left > 0) {
    while (Left >= 4) {
      MmioWrite32(Fifo, *(UINT32*)&Data[i]);
      i += 4;
      Left -= 4;
    }

    while (Left >= 2) {
      MmioWrite16(Fifo, *(UINT16*)&Data[i]);
      i += 2;
      Left -= 2;
    }

    if (Left > 0) {
      MmioWrite8(Fifo, Data[i]);
      Left -= 1;
    }
  }
}
