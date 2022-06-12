#include "Driver.h"

STATIC BOOLEAN Test = FALSE;

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

    DEBUG((EFI_D_INFO, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ DTR=%d RTS=%d\n", !!(Request->Value & 1), !!(Request->Value & 2)));
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

  Driver->CdcReady = FALSE;

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

  return UsbSerialInit(This);
}

STATIC VOID CdcHandleData(
  USB_GADGET *This,
  USB_REQUEST_BLOCK *Urb
) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  EFI_STATUS Status;

  // Transfer should not be queued until buffer is empty.
  ASSERT(Driver->CdcRxBufferDataOffset == 0);

  if (Urb->Actual > 0 && Urb->Status == EFI_SUCCESS) {
    Driver->CdcRxBufferLength = Urb->Actual;
    Test = TRUE;
    // Don't queue transfer yet, will be queued after data is read from buffer.
  } else {
    Status = CdcQueueRead(This);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Failed to queue CDC RX transfer: %r\n", Status));
    }
  }
}

STATIC VOID CdcTxComplete(
  USB_GADGET *This,
  USB_REQUEST_BLOCK *Urb
) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);

  if (EFI_ERROR(Urb->Status))
    DEBUG((EFI_D_INFO, "CDC TX error: %r\n", Urb->Status));
  
  if (Driver->CdcTxCompleteEvent)
    gBS->SignalEvent(Driver->CdcTxCompleteEvent);
}

EFI_STATUS CdcQueueRead(USB_GADGET *This) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  EFI_STATUS Status;
  EFI_TPL OldTpl;

  DEBUG((EFI_D_INFO, "QUEUE READ\n"));

  // Queue may be accessed from interrupt handler.
  OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
  Status = UsbGadgetQueue(
    This,
    Config->CdcDataEpOut.EndpointAddress,
    Driver->CdcDataOutUrb,
    Driver->CdcRxBuffer,
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
  EFI_TPL OldTpl;

  // Queue may be accessed from interrupt handler.
  OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
  Status = UsbGadgetQueue(
    This,
    Config->CdcDataEpIn.EndpointAddress & ~USB_ENDPOINT_DIR_IN,
    Driver->CdcDataInUrb,
    Driver->CdcTxBuffer,
    Driver->CdcTxBufferLength,
    USB_FLAG_TX | USB_FLAG_ZLP,
    CdcTxComplete
  );
  gBS->RestoreTPL(OldTpl);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Flush: failed to queue CDC TX transfer: %r\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

STATIC EFIAPI VOID NullEventHandler(
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context) {}

EFI_STATUS CdcWrite(USB_GADGET *This, IN VOID *Buffer, IN OUT UINTN *BufferSize) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  EFI_STATUS Status;
  //UINTN Index;

  if (!Test) {
    // DEBUG((EFI_D_ERROR, "CDC NOT READY\n"));
    return EFI_SUCCESS;
  }

  UINTN n = MIN(*BufferSize, Driver->CdcTxBufferLength - CDC_DATA_MAX_PACKET);
  if (n == 0) {
    *BufferSize = n;
    ASSERT(0);
    return EFI_TIMEOUT;
  }
  UINT8 *Dest = &Driver->CdcTxBuffer[Driver->CdcTxBufferLength];
  CopyMem(Dest, Buffer, n);
  Driver->CdcTxBufferLength += n;

  if (Driver->CdcTxBufferLength >= CDC_DATA_MAX_PACKET / 2) {
    Status = gBS->CreateEvent(
      EVT_NOTIFY_WAIT,
      TPL_CALLBACK,
      NullEventHandler,
      NULL,
      &Driver->CdcTxCompleteEvent
    );
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Failed to create TX completion event: %r\n", Status));
      ASSERT(0);
      return EFI_DEVICE_ERROR;
    }

    DEBUG((EFI_D_INFO, "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ Flushing ...\n"));
    CdcFlush(This);
  
    while (TRUE) {
      Status = gBS->CheckEvent(Driver->CdcTxCompleteEvent);
      if (Status != EFI_NOT_READY)
        break;
    }

    DEBUG((EFI_D_INFO, "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ Flush complete\n"));
    gBS->CloseEvent(&Driver->CdcTxCompleteEvent);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "WaitForEvent failed: %r\n", Status));
      ASSERT(0);
      return EFI_DEVICE_ERROR;
    }

    Driver->CdcTxBufferLength = 0;
  }

  return EFI_SUCCESS;
}

EFI_STATUS CdcRead(USB_GADGET *This, IN VOID *Buffer, IN OUT UINTN *BufferSize) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  UINTN n;
  EFI_STATUS Status;

  if (Driver->CdcRxBufferLength == 0) {
    *BufferSize = 0;
    return EFI_TIMEOUT;
  }

  // At this point transfer is not queued so buffer accesses are safe.
  n = MIN(*BufferSize, Driver->CdcRxBufferLength);
  CopyMem(Buffer, &Driver->CdcRxBuffer[Driver->CdcRxBufferDataOffset], Driver->CdcRxBufferLength);
  Driver->CdcRxBufferLength -= n;
  if (Driver->CdcRxBufferLength != 0) {
    Driver->CdcRxBufferDataOffset += n;
  } else {
    Driver->CdcRxBufferDataOffset = 0;
    CdcQueueRead(This);
  }

  if (n == *BufferSize)
    Status = EFI_SUCCESS;
  else {
    Status = EFI_TIMEOUT;
    *BufferSize = n;
  }

  return Status;
}
