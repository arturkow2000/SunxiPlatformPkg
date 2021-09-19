#include "Driver.h"
#include "Hw.h"

STATIC VOID UsbEpxRxState(USB_DRIVER *Driver, UINT32 Endpoint, USB_REQUEST_BLOCK *Urb) {
  UINT16 Csr;
  UINT16 Len;
  UINT32 FifoCount;

  // Caller selects endpoint
  Csr = MmioRead16(Driver->Base + MUSB_RXCSR);

  if (Csr & MUSB_RXCSR_P_SENDSTALL)
    return;

  if (Csr & MUSB_RXCSR_RXPKTRDY) {
    Len = MmioRead16(Driver->Base + MUSB_RXCOUNT);

    FifoCount = Urb->Length - Urb->Actual;
    FifoCount = MIN(Len, FifoCount);

    UsbReadFifo(Driver, Endpoint, FifoCount, ((UINT8*)Urb->Buffer + Urb->Actual));
    Urb->Actual += FifoCount;

    /* ack the read! */
    Csr |= MUSB_RXCSR_P_WZC_BITS;
    Csr &= ~MUSB_RXCSR_RXPKTRDY;

    MmioWrite16(Driver->Base + MUSB_RXCSR, Csr);

    /* reach the end or short packet detected */
    if (Urb->Actual == Urb->Length || Len < Driver->Epx[Endpoint - 1].RxPacketSize)
      UsbEpxCompleteRequest(Driver, Urb, EFI_SUCCESS, Endpoint);
  }
}

VOID UsbEpxHandleRxIrq(USB_DRIVER *Driver, UINT32 EndpointNumber) {
  UINT16 Csr;
  USB_REQUEST_BLOCK *Urb;
  LIST_ENTRY *Node;

  if (EndpointNumber >= gSunxiSocConfig.NumEndpoints) {
    DEBUG((EFI_D_ERROR, "RX on non existing endpoint %d\n", EndpointNumber));
    return;
  }

  if (!(Driver->EndpointsUsedForRx & (1u << EndpointNumber))) {
    DEBUG((EFI_D_ERROR, "RX on disabled endpoint %d, ignoring\n", EndpointNumber));
    return;
  }

  Node = GetFirstNode(&Driver->Epx[EndpointNumber - 1].RxQueue);
  if (Node == &Driver->Epx[EndpointNumber - 1].RxQueue)
    return;

  Urb = USB_URB_FROM_LINK(Node);

  UsbSelectEndpoint(Driver, EndpointNumber);
  Csr = MmioRead16(Driver->Base + MUSB_RXCSR);

  if (Csr & MUSB_RXCSR_P_SENTSTALL) {
    Csr |= MUSB_RXCSR_P_WZC_BITS;
    Csr &= ~MUSB_RXCSR_P_SENTSTALL;

    MmioWrite16(Driver->Base + MUSB_RXCSR, Csr);
    return;
  }

  if (Csr & MUSB_RXCSR_P_OVERRUN) {
    Csr &= ~MUSB_RXCSR_P_OVERRUN;
    MmioWrite16(Driver->Base + MUSB_RXCSR, Csr);

    DEBUG((EFI_D_ERROR, "Overrun on EP%d\n", EndpointNumber));
  }

  UsbEpxRxState(Driver, EndpointNumber, Urb);
}

STATIC VOID UsbEpxTxState(USB_DRIVER *Driver, UINT32 Endpoint, USB_REQUEST_BLOCK *Urb) {
  UINT16 Csr;
  UINT16 FifoCount;
  USB_EPX *Ep = &Driver->Epx[Endpoint - 1];

  Csr = MmioRead16(Driver->Base + MUSB_TXCSR);

  FifoCount = MIN(Ep->TxPacketSize, Urb->Length - Urb->Actual);

  if (Csr & MUSB_TXCSR_TXPKTRDY)
    return;

  if (Csr & MUSB_TXCSR_P_SENDSTALL)
    return;

  UsbWriteFifo(Driver, Endpoint, FifoCount, (UINT8*)Urb->Buffer + Urb->Actual);
  Urb->Actual += FifoCount;

  Csr &= ~MUSB_TXCSR_P_UNDERRUN;
  Csr |= MUSB_TXCSR_TXPKTRDY;
  MmioWrite16(Driver->Base + MUSB_TXCSR, Csr);
}

VOID UsbEpxHandleTxIrq(USB_DRIVER *Driver, UINT32 EndpointNumber) {
  UINT16 Csr;
  USB_REQUEST_BLOCK *Urb;
  LIST_ENTRY *Node;
  USB_EPX *Ep;
  UINT32 MaxPacket;

  if (EndpointNumber >= gSunxiSocConfig.NumEndpoints) {
    DEBUG((EFI_D_ERROR, "TX on non existing endpoint %d\n", EndpointNumber));
    return;
  }

  if (!(Driver->EndpointsUsedForRx & (1u << EndpointNumber))) {
    DEBUG((EFI_D_ERROR, "TX on disabled endpoint %d, ignoring\n", EndpointNumber));
    return;
  }

  Ep = &Driver->Epx[EndpointNumber - 1];
  MaxPacket = Ep->TxPacketSize;

  UsbSelectEndpoint(Driver, EndpointNumber);

  if (Csr & MUSB_TXCSR_P_SENTSTALL) {
    Csr |= MUSB_TXCSR_P_WZC_BITS;
    Csr &= ~MUSB_TXCSR_P_SENTSTALL;

    MmioWrite16(Driver->Base + MUSB_TXCSR, Csr);
    return;
  }

  if (Csr & MUSB_TXCSR_P_UNDERRUN) {
    Csr |= MUSB_TXCSR_P_WZC_BITS;
    Csr &= ~(MUSB_TXCSR_P_UNDERRUN | MUSB_TXCSR_TXPKTRDY);

    MmioWrite16(Driver->Base + MUSB_TXCSR, Csr);
    DEBUG((EFI_D_ERROR, "Underrun on EP%d\n", EndpointNumber));
  }

  Node = GetFirstNode(&Driver->Epx[EndpointNumber - 1].TxQueue);
  if (Node == &Driver->Epx[EndpointNumber - 1].TxQueue)
    return;

  Urb = USB_URB_FROM_LINK(Node);

  if ((Urb->Flags & USB_FLAG_ZLP) && Urb->Length > 0 && (Urb->Length % MaxPacket == 0) && (Urb->Actual == Urb->Length)) {
    MmioWrite16(Driver->Base + MUSB_TXCSR, MUSB_TXCSR_MODE | MUSB_TXCSR_TXPKTRDY);
    Urb->Flags &= ~USB_FLAG_ZLP;
  }

  if (Urb->Length == Urb->Actual) {
    UsbEpxCompleteRequest(Driver, Urb, EFI_SUCCESS, EndpointNumber);

    UsbSelectEndpoint(Driver, EndpointNumber);

    Node = GetFirstNode(&Driver->Epx[EndpointNumber - 1].TxQueue);
    if (Node == &Driver->Epx[EndpointNumber - 1].TxQueue)
      return;

    Urb = USB_URB_FROM_LINK(Node);
  }

  UsbEpxTxState(Driver, EndpointNumber, Urb);
}

VOID UsbEpxRestart(USB_DRIVER *Driver, UINT32 Endpoint, USB_REQUEST_BLOCK *Urb) {
  UsbSelectEndpoint(Driver, Endpoint);
  if (Urb->Flags & USB_FLAG_TX) {
    UsbEpxTxState(Driver, Endpoint, Urb);
  } else {
    UsbEpxRxState(Driver, Endpoint, Urb);
  }
}

EFI_STATUS UsbEpxQueue(USB_DRIVER *Driver, UINT32 Endpoint, USB_REQUEST_BLOCK *Urb) {
  BOOLEAN IsTx;
  UINT32 EnabledEpMask;
  BOOLEAN ListEmpty;
  USB_EPX *Ep;

  if (!Urb || Endpoint == 0)
    return EFI_INVALID_PARAMETER;

  if (Endpoint >= gSunxiSocConfig.NumEndpoints)
    return EFI_NOT_FOUND;

  Ep = &Driver->Epx[Endpoint - 1];

  IsTx = !!(Urb->Flags & USB_FLAG_TX);
  if (IsTx)
    EnabledEpMask = Driver->EndpointsUsedForTx;
  else
    EnabledEpMask = Driver->EndpointsUsedForRx;

  if (!(EnabledEpMask & (1u << Endpoint))) {
    DEBUG((
      EFI_D_ERROR,
      "Attempting to queue %s transfer on disabled endpoint %d\n",
      IsTx ? L"TX" : L"RX",
      Endpoint
    ));
    return EFI_INVALID_PARAMETER;
  }

  /// TODO: ensure node is not inserted anywhere else

  Urb->Actual = 0;

  if (IsTx) {
    ListEmpty = IsListEmpty(&Ep->TxQueue);
    InsertTailList(&Ep->TxQueue, &Urb->Node);
  }
  else {
    ListEmpty = IsListEmpty(&Ep->RxQueue);
    InsertTailList(&Ep->RxQueue, &Urb->Node);
  }

  if (!Ep->Busy && ListEmpty) {
    UsbEpxRestart(Driver, Endpoint, Urb);
  }

  return EFI_SUCCESS;
}

EFI_STATUS UsbEnableEndpoint(USB_DRIVER *Driver, USB_ENDPOINT_DESCRIPTOR *Descriptor) {
  UINT8 Endpoint;
  BOOLEAN IsIn;
  UINT16 MaxPacketSize;
  UINT16 HardwareFifoSize;
  UINT16 Csr;
  UINT8 TransferType;

  if (!Descriptor)
    return EFI_INVALID_PARAMETER;

  if (Descriptor->DescriptorType != USB_DESC_TYPE_ENDPOINT)
    return EFI_INVALID_PARAMETER;

  Endpoint = Descriptor->EndpointAddress & ~USB_ENDPOINT_DIR_IN;
  if (Endpoint == 0)
    return EFI_INVALID_PARAMETER;

  if (Endpoint >= gSunxiSocConfig.NumEndpoints)
    return EFI_NOT_FOUND;

  IsIn = !!(Descriptor->EndpointAddress & USB_ENDPOINT_DIR_IN);

  TransferType = Descriptor->Attributes & 3;

  if (Descriptor->MaxPacketSize & ~0x7ff) {
    DEBUG((EFI_D_ERROR, "High bandwidth isochronous transfers are not supported\n"));
    return EFI_DEVICE_ERROR;
  }

  MaxPacketSize = Descriptor->MaxPacketSize & 0x7ff;

  if (IsIn) {
    if (Driver->EndpointsUsedForTx & (1u << (UINT32)Endpoint))
      return EFI_ALREADY_STARTED;

    HardwareFifoSize = 1u << (UINT16)gSunxiSocConfig.EpFifoConfig[Endpoint].TxMaxPacketSizeLog2;
  }
  else {
    if (Driver->EndpointsUsedForRx & (1u << (UINT32)Endpoint))
      return EFI_ALREADY_STARTED;

    HardwareFifoSize = 1u << (UINT16)gSunxiSocConfig.EpFifoConfig[Endpoint].RxMaxPacketSizeLog2;
  }

  if (MaxPacketSize > HardwareFifoSize) {
    DEBUG((EFI_D_ERROR, "Packet size beyond hardware FIFO size (%u > %u)\n", MaxPacketSize, HardwareFifoSize));
    return EFI_INVALID_PARAMETER;
  }

  UsbSelectEndpoint(Driver, Endpoint);
  if (IsIn) {
    if (!(gSunxiSocConfig.EpFifoConfig[Endpoint].Flags & USB_FIFO_SHARED) && Driver->EndpointsUsedForRx & (1u << (UINT32)Endpoint))
      return EFI_ALREADY_STARTED;

    /* Set TXMAXP with the FIFO size of the endpoint
     * to disable double buffering mode.
     */
    MmioWrite16(Driver->Base + MUSB_TXMAXP, MaxPacketSize);
    Csr = MUSB_TXCSR_MODE | MUSB_TXCSR_CLRDATATOG;

    if (MmioRead16(Driver->Base + MUSB_TXCSR) & MUSB_TXCSR_FIFONOTEMPTY)
      Csr |= MUSB_TXCSR_FLUSHFIFO;

    if (TransferType == USB_ENDPOINT_ISO)
      Csr |= MUSB_TXCSR_P_ISO;

    MmioWrite16(Driver->Base + MUSB_TXCSR, Csr);

    Driver->EndpointsUsedForTx |= 1u << (UINT32)Endpoint;
    Driver->Epx[Endpoint - 1].TxPacketSize = MaxPacketSize;
  } else {
    if (!(gSunxiSocConfig.EpFifoConfig[Endpoint].Flags & USB_FIFO_SHARED) && Driver->EndpointsUsedForTx & (1u << (UINT32)Endpoint))
      return EFI_ALREADY_STARTED;

    /* Set RXMAXP with the FIFO size of the endpoint
     * to disable double buffering mode.
     */
    MmioWrite16(Driver->Base + MUSB_RXMAXP, MaxPacketSize);

    /* force shared fifo to OUT-only mode */
    if (gSunxiSocConfig.EpFifoConfig[Endpoint].Flags & USB_FIFO_SHARED)
      MmioAnd16(Driver->Base + MUSB_TXCSR, ~(MUSB_TXCSR_MODE | MUSB_TXCSR_TXPKTRDY));

    Csr = MUSB_RXCSR_FLUSHFIFO | MUSB_RXCSR_CLRDATATOG;

    if (TransferType == USB_ENDPOINT_ISO)
      Csr |= MUSB_RXCSR_P_ISO;
    else if (TransferType == USB_ENDPOINT_INTERRUPT)
      Csr |= MUSB_RXCSR_DISNYET;

    MmioWrite16(Driver->Base + MUSB_RXCSR, Csr);

    Driver->EndpointsUsedForRx |= 1u << (UINT32)Endpoint;
    Driver->Epx[Endpoint - 1].RxPacketSize = MaxPacketSize;
  }

  return EFI_SUCCESS;
}

VOID UsbEpxCompleteRequest(USB_DRIVER *Driver, USB_REQUEST_BLOCK *Urb, EFI_STATUS Status, UINT32 Endpoint) {
  RemoveEntryList(&Urb->Node);
  UsbSignalCompletion(Driver, Endpoint, Urb, Status);
}