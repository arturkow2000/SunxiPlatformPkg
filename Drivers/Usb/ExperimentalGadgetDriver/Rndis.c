#include "Driver.h"

STATIC VOID RndisHandleData(
  USB_GADGET *This,
  USB_REQUEST_BLOCK *Urb
) {
  if (Urb->Status == EFI_ABORTED)
    return;

  DEBUG((EFI_D_INFO, "Incoming RNDIS data: len=%d status=%r\n", Urb->Actual, Urb->Status));
  ASSERT(0);
}

EFI_STATUS RndisEnable(USB_GADGET *This) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  EFI_STATUS Status;

  Status = UsbGadgetEnableEndpoint(This, &Config->RndisControlEp);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to enable RNDIS control endpoint\n"));
    return Status;
  }

  Status = UsbGadgetEnableEndpoint(This, &Config->RndisDataEpOut);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to enable RNDIS data out endpoint\n"));
    return Status;
  }

  Status = UsbGadgetEnableEndpoint(This, &Config->RndisDataEpIn);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to enable RNDIS data in endpoint\n"));
    return Status;
  }

  Status = UsbGadgetQueue(
    This,
    Config->RndisDataEpOut.EndpointAddress,
    Driver->RndisDataOutUrb,
    Driver->RndisRxBufferTemp,
    CDC_DATA_MAX_PACKET,
    0,
    RndisHandleData
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to initialize RNDIS data transfer\n"));
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS RndisDisable(USB_GADGET *This) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  EFI_STATUS Status;

  Status = UsbGadgetDisableEndpoint(This, &Config->RndisControlEp);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to disable RNDIS control endpoint\n"));
  }

  Status = UsbGadgetDisableEndpoint(This, &Config->RndisDataEpIn);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to disable RNDIS data in endpoint\n"));
  }

  Status = UsbGadgetDisableEndpoint(This, &Config->RndisDataEpOut);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to disable RNDIS data out endpoint\n"));
  }

  return EFI_SUCCESS;
}

STATIC VOID RndisDebugTxComplete(
  USB_GADGET *This,
  USB_REQUEST_BLOCK *Urb
) {
  DEBUG((EFI_D_ERROR, "+++++++++++++++++++++++ RNDIS TX complete, status: %r\n", Urb->Status));
}

STATIC VOID RndisHandleCommand(
  USB_GADGET *This,
  USB_REQUEST_BLOCK *Urb
) {
  STATIC UINT8 RndisInterruptMsg[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  DEVICE_CONFIG *Config = (DEVICE_CONFIG*)Driver->ConfigDescriptor;
  EFI_STATUS Status;

  if (Urb->Status == EFI_ABORTED)
    return;

  if (Urb->Status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "RNDIS control transfer failed\n"));
    return;
  }

  DEBUG((EFI_D_ERROR, "\n\n\n\nRNIDS INCOMING DATA :):):):):):):):):)\n\n\n\n"));

  RNDIS_MESSAGE_HEADER *Header = (RNDIS_MESSAGE_HEADER*)Driver->RndisEncapsulatedMessage;
  if (Header->Length > Urb->Actual) {
    DEBUG((EFI_D_ERROR, "RNDIS message bigger than control transfer size\n"));
    return;
  }
  DEBUG((EFI_D_INFO, "RNDIS Type=%d Length=%d\n", Header->Type, Header->Length));
  switch (Header->Type) {
  case REMOTE_NDIS_INITIALIZE_MSG:
    if (Header->Length < sizeof(RNDIS_INITIALIZE_MSG))
      goto error;

    RNDIS_INITIALIZE_MSG *Init = (RNDIS_INITIALIZE_MSG*)Header;
    DEBUG((EFI_D_INFO, "RNDIS init version %d.%d, max transfer=%d\n", Init->Major, Init->Minor, Init->MaxTransferSize));

    RNDIS_INITIALIZE_CMPLT *InitCmplt = (RNDIS_INITIALIZE_CMPLT*)Header;
    InitCmplt->Header.Type = REMOTE_NDIS_INITIALIZE_CMPLT;
    InitCmplt->Header.Length = sizeof(RNDIS_INITIALIZE_CMPLT);
    InitCmplt->RequestId = Init->RequestId;
    InitCmplt->Major = 1;
    InitCmplt->Minor = 0;
    InitCmplt->DeviceFlags = RNDIS_DF_CONNECTIONLESS;
    InitCmplt->Medium = RNDIS_MEDIUM_802_3;
    InitCmplt->MaxPacketsPerMessage = 1;
    InitCmplt->PacketAlignmentFactor = 0;
    InitCmplt->AFListOffset = 0;
    InitCmplt->AFListSize = 0;
    Driver->RndisEncTxBufferSize = sizeof(RNDIS_INITIALIZE_CMPLT);
    break;
  default:
error:
    DEBUG((EFI_D_ERROR, "Unhandled RNDIS message Type=%d Length=%d\n", Header->Type, Header->Length));
    // RNDIS expects 1 byte message filled with zeros instead of stall.
    Driver->RndisEncTxBufferSize = 1;
    Driver->RndisEncapsulatedMessage[0] = 0;
    break;
  }

  // Host won't poll control endpoint until we send interrupt.
  Status = UsbGadgetQueue(
    This,
    Config->RndisControlEp.EndpointAddress & ~USB_ENDPOINT_DIR_IN,
    Driver->RndisInterruptUrb,
    RndisInterruptMsg,
    sizeof RndisInterruptMsg,
    USB_FLAG_TX,
    RndisDebugTxComplete
  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Failed to send RNDIS interrupt (%r), expect RNDIS to break\n", Status));
  }
}

EFI_STATUS RndisHandleRequest(USB_GADGET *This, USB_DEVICE_REQUEST *Request) {
  GADGET_DRIVER *Driver = GADGET_TO_DRIVER(This);
  UINT32 TotalLength;

  if (Request->RequestType & USB_ENDPOINT_DIR_IN) {
    if (Driver->RndisEncTxBufferSize == 0) {
      DEBUG((EFI_D_ERROR, "RNDIS host issued IN transfer but we have nothing to say, stalling\n"));
      return EFI_DEVICE_ERROR;
    }

    TotalLength = MIN(Request->Length, Driver->RndisEncTxBufferSize);
    return UsbGadgetInitUrb(
      This,
      This->ControlUrb,
      Driver->RndisEncapsulatedMessage,
      TotalLength,
      TotalLength == Request->Length ? USB_FLAG_ZLP : 0 | USB_FLAG_TX,
      NULL
    );
  } else {
    if (Request->Length > RNDIS_ENC_MAX) {
      DEBUG((EFI_D_ERROR, "RNDIS request too big (%d, max is %d)\n", Request->Length, RNDIS_ENC_MAX));
      return EFI_DEVICE_ERROR;
    }

    if (Request->Length < sizeof(RNDIS_MESSAGE_HEADER)) {
      DEBUG((EFI_D_ERROR, "RNDIS message smaller than header\n"));
      return EFI_DEVICE_ERROR;
    }

    return UsbGadgetInitUrb(
      This,
      This->ControlUrb,
      Driver->RndisEncapsulatedMessage,
      Request->Length,
      0,
      RndisHandleCommand
    );
  }
}
