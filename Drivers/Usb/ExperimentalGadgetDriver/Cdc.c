#include "Driver.h"

STATIC VOID CdcHandleLineCoding(USB_GADGET *This, USB_REQUEST_BLOCK *Urb) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  USB_CDC_LINE_CODING *LineCoding = &Driver->CdcState.PendingLineCoding;

  if (Urb->Length != sizeof(USB_CDC_LINE_CODING)) {
    DEBUG((EFI_D_ERROR, "Invalid CDC set line coding request: expected len=%d, got %d\n", Urb->Length, sizeof(USB_CDC_LINE_CODING)));
    UsbGadgetHaltEndpoint(This, 0);
    return;
  }

  DEBUG((EFI_D_INFO, "Rate: %d\n", LineCoding->RteRate));
  switch(LineCoding->ParityType) {
  case USB_CDC_NO_PARITY:
    DEBUG((EFI_D_INFO, "Parity: None\n"));
    break;
  case USB_CDC_ODD_PARITY:
    DEBUG((EFI_D_INFO, "Parity: Odd\n"));
    break;
  case USB_CDC_EVEN_PARITY:
    DEBUG((EFI_D_INFO, "Parity: Even\n"));
    break;
  case USB_CDC_MARK_PARITY:
    DEBUG((EFI_D_INFO, "Parity: Mark\n"));
    break;
  case USB_CDC_SPACE_PARITY:
    DEBUG((EFI_D_INFO, "Parity: Space\n"));
    break;
  default:
    DEBUG((EFI_D_INFO, "Parity: Unknown\n"));
    break;
  }
  switch(LineCoding->CharFormat) {
  case USB_CDC_1_STOP_BITS:
    DEBUG((EFI_D_INFO, "Stop bits: 1\n"));
    break;
  case USB_CDC_1_5_STOP_BITS:
    DEBUG((EFI_D_INFO, "Stop bits: 1.5\n"));
    break;
  case USB_CDC_2_STOP_BITS:
    DEBUG((EFI_D_INFO, "Stop bits: 2\n"));
    break;
  default:
    DEBUG((EFI_D_INFO, "Stop bits: unknown\n"));
    break;
  }
  DEBUG((EFI_D_INFO, "Data bits: %d\n", LineCoding->DataBits));

  CopyMem(&Driver->CdcState.LineCoding, &Driver->CdcState.PendingLineCoding, sizeof(USB_CDC_LINE_CODING));
}

EFI_STATUS CdcHandleRequest(USB_GADGET *This, USB_DEVICE_REQUEST *Request) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);

  switch (Request->Request)
  {
  case USB_CDC_REQ_SET_LINE_CODING:
    if (Request->RequestType & USB_ENDPOINT_DIR_IN)
      return EFI_DEVICE_ERROR;

    return UsbGadgetInitUrb(This, This->ControlUrb, &Driver->CdcState.PendingLineCoding, sizeof(USB_CDC_LINE_CODING), 0, CdcHandleLineCoding);
  
  case USB_CDC_REQ_GET_LINE_CODING:
    if (!(Request->RequestType & USB_ENDPOINT_DIR_IN))
      return EFI_DEVICE_ERROR;

    return UsbGadgetInitUrb(This, This->ControlUrb, &Driver->CdcState.LineCoding, sizeof(USB_CDC_LINE_CODING), 0, NULL);

  case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
    if (!(Request->RequestType & USB_ENDPOINT_DIR_IN))
      return EFI_DEVICE_ERROR;

    // Not implemented, CDC works fine without this.
    DEBUG((EFI_D_INFO, "DTR=%d RTS=%d\n", !!(Request->Value & 1), !!(Request->Value & 2)));
    return EFI_SUCCESS;
  
  default:
    DEBUG((EFI_D_ERROR, "Unknown CDC request 0x%d\n", Request->Request));
    return EFI_DEVICE_ERROR;
  }
}

EFI_STATUS CdcEnable(USB_GADGET *This) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  EFI_STATUS Status;

  Driver->CdcTxPending = FALSE;

  Status = UsbGadgetEnableEndpoint(This, &Config->CdcControlEp);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to enable CDC control endpoint\n"));
    return Status;
  }

  Status = UsbGadgetEnableEndpoint(This, &Config->CdcDataEpOut);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to enable CDC data out endpoint\n"));
    return Status;
  }

  Status = UsbGadgetEnableEndpoint(This, &Config->CdcDataEpIn);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to enable CDC data in endpoint\n"));
    return Status;
  }

  Status = CdcQueueRead(This);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to initialize CDC data transfer\n"));
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS CdcDisable(USB_GADGET *This) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  EFI_STATUS Status;

  Status = UsbGadgetDisableEndpoint(This, &Config->CdcControlEp);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to disable CDC control endpoint\n"));
  }

  Status = UsbGadgetDisableEndpoint(This, &Config->CdcDataEpIn);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to disable CDC data in endpoint\n"));
  }

  Status = UsbGadgetDisableEndpoint(This, &Config->CdcDataEpOut);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to disable CDC data out endpoint\n"));
  }

  return EFI_SUCCESS;
}

BOOLEAN CdcIsEnabled(USB_GADGET *This) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  return Driver->ActiveConfig == 1;
}

STATIC VOID CdcHandleData(
  USB_GADGET *This,
  USB_REQUEST_BLOCK *Urb
) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  EFI_STATUS Status;
  UINTN Length;

  if (Urb->Actual > 0 && Urb->Status == EFI_SUCCESS) {
    Length = Urb->Actual;
    SimpleBufferWrite(&Driver->CdcRxBuffer, Driver->CdcRxBufferTemp, &Length);
    if (Length != Urb->Actual) {
      SimpleBufferDiscard(&Driver->CdcRxBuffer, Urb->Actual - Length);
      SimpleBufferWrite(&Driver->CdcRxBuffer, &Driver->CdcRxBufferTemp[Length], &Length);
    }
  }

  if (Urb->Status == EFI_ABORTED) {
    // Request gets aborted when we disable CDC either do to configuration
    // change or USB reset. Don't queue more transfers, CDC has to be re-enabled
    // first.
    return;
  }

  Status = CdcQueueRead(This);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to queue CDC RX transfer: %r\n", Status));
  }
}

STATIC VOID CdcTxComplete(
  USB_GADGET *This,
  USB_REQUEST_BLOCK *Urb
) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);

  if (Urb->Status == EFI_ABORTED)
    return;

  if (EFI_ERROR(Urb->Status))
    DEBUG((EFI_D_INFO, "CDC TX error: %r\n", Urb->Status));

  SimpleBufferDiscard(&Driver->CdcTxBuffer, Urb->Length);
  Driver->CdcTxPending = FALSE;
  CdcFlush(This);
}

EFI_STATUS CdcQueueRead(USB_GADGET *This) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  EFI_STATUS Status;
  EFI_TPL OldTpl;

  // Queue may be accessed from interrupt handler.
  OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
  Status = UsbGadgetQueue(
    This,
    Config->CdcDataEpOut.EndpointAddress,
    Driver->CdcDataOutUrb,
    Driver->CdcRxBufferTemp,
    CDC_DATA_MAX_PACKET,
    0,
    CdcHandleData
  );
  gBS->RestoreTPL(OldTpl);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to queue CDC RX transfer: %r\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS CdcFlush(USB_GADGET *This) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  EFI_STATUS Status;
  UINT8 *Data;
  UINTN Length;

  if (Driver->CdcTxPending)
    return EFI_NOT_READY;

  SimpleBufferPeek(&Driver->CdcTxBuffer, &Data, &Length, NULL, NULL);
  if (Length == 0) {
    return EFI_SUCCESS;
  }

  Status = UsbGadgetQueue(
    This,
    Config->CdcDataEpIn.EndpointAddress & ~USB_ENDPOINT_DIR_IN,
    Driver->CdcDataInUrb,
    Data,
    Length,
    USB_FLAG_TX | USB_FLAG_ZLP,
    CdcTxComplete
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Flush: failed to queue CDC TX transfer: %r\n", Status));
    return Status;
  }
  Driver->CdcTxPending = TRUE;

  return Driver->CdcTxPending ? EFI_SUCCESS : EFI_NOT_READY;
}

EFI_STATUS CdcWrite(USB_GADGET *This, IN VOID *Buffer, IN OUT UINTN *BufferSize) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  UINTN TotalSize = *BufferSize;
  UINTN Length = TotalSize;
  UINTN Offset;
  EFI_TPL OldTpl;

  // Discard any data if not enabled.
  if (!CdcIsEnabled(This))
    return EFI_SUCCESS;

  if (TotalSize == 0)
    return EFI_SUCCESS;

  // Buffer may be accessed from interrupt handler. We don't care about
  // overriding the data inside, but metadata could possibly be corrupted.
  OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
  SimpleBufferWrite(&Driver->CdcTxBuffer, Buffer, &Length);
  if (Length != TotalSize) {
    Offset = Length;
    Length = TotalSize - Length;
    SimpleBufferDiscard(&Driver->CdcTxBuffer, Length);
    // We cannot reliably determine whether there is someone listening on the
    // other side. If there isn't anyone transfers may be halted forever so if
    // nobody read our data discard it now.
    SimpleBufferWrite(&Driver->CdcTxBuffer, &((UINT8*)Buffer)[Offset], &Length);
  }

  CdcFlush(This);
  gBS->RestoreTPL(OldTpl);

  return TotalSize == *BufferSize ? EFI_SUCCESS : EFI_TIMEOUT;
}

EFI_STATUS CdcRead(USB_GADGET *This, IN VOID *Buffer, IN OUT UINTN *BufferSize) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  EFI_TPL OldTpl;
  UINTN Total;

  if (*BufferSize == 0)
    return EFI_SUCCESS;

  OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
  Total = SimpleBufferRead(&Driver->CdcRxBuffer, Buffer, *BufferSize);
  gBS->RestoreTPL(OldTpl);

  return Total != 0 ? EFI_SUCCESS : EFI_TIMEOUT;
}
